/* See LICENSE file for license details. */


#include <cstddef>
#include <cstdlib>
#include <err.h>        // errx()
#include <errno.h>      // number of errors
#include <unistd.h>     // write()
#include <iostream>
#include <X11/Xlib.h>   // XOpenDisplay()
#include "argparser.h"
#include "logger.h"


/* Start parameters.  Might be altered by the argument parser. */
bool verbose    = false;
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
static void dontkillme( void )
{
#define OOM "/proc/self/oom_score_adj"
	int fd = open( OOM, O_WRONLY );

	if ( -1 == fd )
  {
    /* If the file does not exist, return. */
    if ( ENOENT == errno )
    {
      Logger::get()->log( Logger::VERBOSE,
          "/proc/self/oom_score_adj does not exist (OK)" );
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

  Logger::create( logfile );
  Logger::get()->log( "Launched ", *argv );

#ifdef __linux__
  /* Disable the Out-of-memory killer for this process. */
	dontkillme();
#endif

	if ( ! XOpenDisplay( 0 ) )
		errx( EXIT_FAILURE, "cannot open display" );

	exit( EXIT_SUCCESS );
}
