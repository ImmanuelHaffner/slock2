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
#include <iostream>

#include <unistd.h>     // write(), getuid()
#include <pwd.h>        // getpwuid()
#include <X11/Xlib.h>   // XOpenDisplay()

#if HAVE_BSD_AUTH
#include <login_cap.h>
#include <bsd_auth.h>
#endif

#include "argparser.h"
#include "logger.h"
#include "lock.h"


/* Start parameters.  Might be altered by the argument parser. */
Logger::LogLevel logLevel = Logger::LL_Normal;
#ifndef LOGFILE
#define LOGFILE "/var/log/slock.log"
#endif
char const *logfile   = LOGFILE;


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
    errx( EXIT_FAILURE,
        "cannot disable the out-of-memory killer for this process: "
        "could not open " OOM );
  }

  /* If the file could not be opened for write-only, or the file could not be
   * written, or the file could not be closed, exit with failure.
   */
  if ( write( fd, "-1000\n", 6 ) != 6 )
    errx( EXIT_FAILURE,
        "cannot disable the out-of-memory killer for this process: "
        "could not write to " OOM );

  if ( close( fd ) )
    errx( EXIT_FAILURE,
        "cannot disable the out-of-memory killer for this process: "
        "could not close " OOM );
}
#endif


int main( int, char **argv )
{
  parseArguments( argv );

  Logger::create( logfile, logLevel );
  Logger::get()->l( "Launched ", *argv );

#ifdef __linux__
  /* Disable the Out-of-memory killer for this process. */
  disableOOMKiller();
#endif


  /* Verify that the user has a password set. */
  uid_t UID = getuid();
  if ( ! getpwuid( UID ) )
  {
    Logger::get()->e( "no password entry for UID ", UID );
    exit( EXIT_FAILURE );
  }

  /* Connect to the X Server. */
  Display *display = XOpenDisplay( 0 );
  if ( ! display )
  {
    Logger::get()->e( "cannot open display" );
    exit( EXIT_FAILURE );
  }

  /* Obtain a lock for every screen. */
  int const nscreens = ScreenCount( display );
  Lock *locks = new Lock[ sizeof( *locks ) * nscreens ];

  if ( ! locks )
  {
    Logger::get()->e( "failed to allocate locks: ",
        strerror( errno ) );
    exit( EXIT_FAILURE );
  }

  for ( int screen = 0; screen < nscreens; ++screen )
  {
    locks[ screen ].screen = screen;
    init( display, &locks[ screen ] );
    if ( lock( display, &locks[ screen ] ) )
      Logger::get()->d( "locked screen ", screen );
    else
      Logger::get()->d( "failed to lock screen ", screen );
  }

  XSync( display, False );

  exit( EXIT_SUCCESS );
}
