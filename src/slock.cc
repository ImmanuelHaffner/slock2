/* See LICENSE file for license details. */


/*
 * 500 - X/Open 5, incorporating POSIX 1995
 * 600 - X/Open 6, incorporating POSIX 2004
 * 700 - X/Open 7, incorporating POSIX 2008
 */
#define _XOPEN_SOURCE 500

#if HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <cstddef>      // EXIT_FAILURE, EXIT_SUCCESS
#include <cstdlib>      // exit()
#include <err.h>        // errx()
#include <errno.h>      // number of errors
#include <string>       // to_string
#include <iostream>

#include <unistd.h>     // write(), getuid()
#include <pwd.h>        // getpwuid()
#include <X11/Xlib.h>   // XOpenDisplay()
#include <X11/keysym.h>
#include <X11/Xutil.h>

#if HAVE_BSD_AUTH
#include <login_cap.h>
#include <bsd_auth.h>
#endif

#include "argparser.h"
#include "logger.h"
#include "lock.h"
#include "event.h"


/* Start parameters.  Might be altered by the argument parser. */
Logger::LogLevel logLevel = Logger::LL_Normal;
char const *logfile   = LOGFILE;
bool enableBell       = false;


#ifdef __linux__
#include <fcntl.h>      // open()

/**
 * Disables the Out-of-memory killer for this process.
 *
 * Needs root privileges.
 */
static void disableOOMKiller( void )
{
#define OOM "/proc/self/oom_score_adj"
  int fd = open( OOM, O_WRONLY );

  if ( -1 == fd )
  {
    /* If the file does not exist, return. */
    if ( ENOENT == errno )
    {
      Logger::get()->d( "/proc/self/oom_score_adj does not exist (OK)" );
      return;
    }

    /* If the file could not be opened, fail. */
    Logger::get()->e(
        "Cannot disable the out-of-memory killer for this process: "
        "could not open " OOM );
    exit( EXIT_FAILURE );
  }

  /* If the file could not be opened for write-only, or the file could not be
   * written, or the file could not be closed, exit with failure.
   */
  if ( write( fd, "-1000\n", 6 ) != 6 )
  {
    Logger::get()->e(
        "cannot disable the out-of-memory killer for this process: "
        "could not write to " OOM );
    exit( EXIT_FAILURE );
  }

  if ( close( fd ) )
  {
    Logger::get()->e(
        "cannot disable the out-of-memory killer for this process: "
        "could not close " OOM );
    exit( EXIT_FAILURE );
  }
}
#endif


/**
 * If BSD Xauth is not available, get the password in Linux-style.
 */
#ifndef HAVE_BSD_AUTH
static const char * getpw() /* only run as root */
{
  const char *rval;
  struct passwd *pw;

  errno = 0;
  pw = getpwuid(getuid());
  if (errno)
  {
    Logger::get()->e( "getpwuid ", strerror( errno ) );
    exit( EXIT_FAILURE );
  }
  else if ( ! pw )
  {
    Logger::get()->e(
        "cannot retrieve password entry (make sure to suid or sgid slock)" );
    exit( EXIT_FAILURE );
  }

  endpwent();
  rval =  pw->pw_passwd;

#if HAVE_SHADOW_H
  if (rval[0] == 'x' && rval[1] == '\0') {
    struct spwd *sp;
    sp = getspnam(getenv("USER"));
    if ( ! sp )
    {
      Logger::get()->e(
          "cannot retrieve shadow entry (make sure to suid or sgid slock)" );
      exit( EXIT_FAILURE );
    }

    endspent();
    rval = sp->sp_pwdp;
  }
#endif

  /* drop privileges */
  if ( geteuid() == 0 &&
      ( (getegid() != pw->pw_gid && setgid(pw->pw_gid) < 0) ||
        setuid(pw->pw_uid) < 0 ) )
  {
    Logger::get()->e( "cannot drop privileges" );
    exit( EXIT_FAILURE );
  }

  return rval;
}
#endif


/**
 * Catch XEvents and read in the password.
 * If the correct password is entered, the cuntion returns.
 */
static void
#ifdef HAVE_BSD_AUTH
readpw( Display *display, Lock locks[], int nscreens )
#else
readpw( Display *display, Lock locks[], int nscreens, const char *pws )
#endif
{
  char buf[32];
  /* A buffer for the entered password. */
  char passwd[256];
  /* Length of the entered password */
  unsigned len = 0;
  unsigned lastLen = len;
  KeySym ksym;
  XEvent ev;

  /* Counts the number of failed login attempts. */
  unsigned fails = 0;

  bool running = true;
  while ( running && ! XNextEvent( display, &ev ) )
  {
    if ( KeyPress != ev.type )
    {
      for ( int i = 0; i < nscreens; ++i )
        XRaiseWindow( display, locks[ i ].win );
      continue;
    }

    buf[ 0 ] = 0; // string terminator

    int num =
      XLookupString(
          &ev.xkey,       /* XKeyEvent */
          buf,            /* return buffer */
          sizeof( buf ),  /* buffer bytes */
          &ksym,          /* KeySym return */
          NULL            /* XComposeStatus */
          );


    /* Translate keys from notepad to regular keys. */
    if ( IsKeypadKey( ksym ) )
    {
      if ( XK_KP_Enter == ksym )
        ksym = XK_Return;
      else if ( XK_KP_0 <= ksym && ksym <= XK_KP_9)
        ksym = ( ksym - XK_KP_0 ) + XK_0;
    }

    /* Ignore special keys. */
    if ( IsFunctionKey( ksym ) || IsKeypadKey( ksym ) ||
        IsMiscFunctionKey( ksym ) || IsPFKey( ksym ) ||
        IsPrivateKeypadKey( ksym ) )
      continue;

    switch ( ksym )
    {
      case XK_Return:
        Logger::get()->d( "XEvent: Return" );
        passwd[ len ] = 0; // string terminator

#ifdef HAVE_BSD_AUTH
        running = ! auth_userokay( getlogin(), NULL, "auth-xlock", passwd );
#else
        running = ! streq( crypt( passwd, pws ), pws );
#endif

        if ( ! running )
        {
          Logger::get()->l( "Correct password entered" );
          loginSuccess();
          goto end;
        }

        ++fails;
        Logger::get()->l( "Incorrect password entered (#", fails, ")" );
        loginFail();

        if ( enableBell )
          XBell( display, 100 );

        len = 0;  // set length of entered password to 0
        break;

      case XK_Escape:
        Logger::get()->d( "XEvent: Escape" );
        len = 0;  // set length of entered password to 0
        break;

      case XK_BackSpace:
        Logger::get()->d( "XEvent: BackSpace" );
        if ( len )
          --len;
        break;

      default:
        {
          if ( ! num )
            break;

          /* Ignore control keys. */
          if ( iscntrl( (int) buf[ 0 ] ) )
            break;

          /* Append the read bytes to the password. */
          if ( len + num < sizeof( passwd ) ) {
            memcpy( passwd + len, buf, num );
            len += num;
          }
          else
            Logger::get()->w( "Input exceeds password buffer" );
        }
        break;
    } // end switch ( ksym )

    if ( 0 == lastLen && 0 != len ) // characters entered
    {
      for ( int i = 0; i < nscreens; ++i )
      {
        XSetWindowBackground( display, locks[ i ].win,
            locks[ i ].colorActive.pixel );
        XClearWindow( display, locks[ i ].win );
      }
    }
    else if ( ! fails && 0 != lastLen && 0 == len )  // all characters removed
      for ( int i = 0; i < nscreens; ++i )
      {
        /* Set the background color. */
        XSetWindowBackground( display, locks[ i ].win,
            locks[ i ].colorInactive.pixel );
        XClearWindow( display, locks[ i ].win );
      }
    else if ( fails && 0 == len )
    {
      std::string str = std::to_string( fails );
      for ( int i = 0; i < nscreens; ++i )
      {
        /* Set the background color. */
        XSetWindowBackground( display, locks[ i ].win,
            locks[ i ].colorError.pixel );
        XClearWindow( display, locks[ i ].win );

        /* Draw the number of failed login attempts. */
        XDrawString( display, locks[ i ].win,
            locks[ i ].gc,
            10,  /* X */
            10,  /* Y */
            str.c_str(),
            str.length() );
      }
    }

    lastLen = len;
  } // end while
end:;
}


int main( int, char **argv )
{
  parseArguments( argv );

  Logger::create( logfile, logLevel );
  if ( atexit( Logger::destroy ) )
    Logger::get()->w( "Could not register atexit( Logger::destroy() )" );

  Logger::get()->l( "Launched ", *argv );

#ifdef __linux__
  /* Disable the Out-of-memory killer for this process. */
  disableOOMKiller();
#endif


  /* Verify that the user has a password set. */
  uid_t UID = getuid();
  if ( ! getpwuid( UID ) )
  {
    Logger::get()->e( "No password entry for UID ", UID );
    exit( EXIT_FAILURE );
  }

  /* Connect to the X Server. */
  Display *display = XOpenDisplay( 0 );
  if ( ! display )
  {
    Logger::get()->e( "Cannot open a connection to X Server" );
    exit( EXIT_FAILURE );
  }

  /* Obtain a lock for every screen. */
  int const nscreens = ScreenCount( display );
  Lock *locks = new Lock[ sizeof( *locks ) * nscreens ];

  if ( ! locks )
  {
    Logger::get()->e( "Failed to allocate locks: ", strerror( errno ) );
    exit( EXIT_FAILURE );
  }

  int nlocks = 0;
  for ( int screen = 0; screen < nscreens; ++screen )
  {
    locks[ screen ].screen = screen;
    init( display, &locks[ screen ] );
    lock( display, &locks[ screen ] );
    if ( locks[ screen ].ok )
    {
      ++nlocks;
      Logger::get()->d( "locked screen ", screen );
    }
    else
      Logger::get()->d( "failed to lock screen ", screen );
  }

  XSync( display, False );
  Logger::get()->d( "synced display" );

  if ( ! nlocks )
  {
    delete locks;
    XCloseDisplay( display );
    Logger::get()->e( "No locks have been acquired" );
    exit( EXIT_FAILURE );
  }

#ifdef HAVE_BSD_AUTH
  readpw( display, locks, nscreens )
#else
    readpw( display, locks, nscreens, getpw() );
#endif

  for ( int i = 0; i < nscreens; ++i )
    if ( locks[ i ].ok )
      unlock( display, &locks[ i ] );   // deletes the lock
  Logger::get()->d( "all screens have been unlocked" );

  delete locks;
  XCloseDisplay( display );

  Logger::get()->l( "Exiting.\n---------------------------------------------" );
  exit( EXIT_SUCCESS );
}
