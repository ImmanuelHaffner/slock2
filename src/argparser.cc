#include "argparser.h"

#include <cstddef>
#include <cstdlib>
#include <err.h>
#include <iostream>
#include "util.h"


void usage()
{
  std::cout <<
    "slock2\n"
    "  -h  --help             - prints this message\n"
    "  -b  --bell             - sound signal on failed login\n"
    "  -s  --suspend          - suspend the machine after locking\n"
    "  -v  --verbose          - be verbose\n"
    "      --debug            - produce debug messages\n"
    "      --logfile <FILE>   - sets the log file (default: " LOGFILE ")\n"
    "      --version          - report the version\n"
    ;
  std::cout.flush();
}

void parseArguments( char **argv )
{
  ++argv; // skip method name
  for ( char *arg; ( arg = *argv ); ++argv )
  {
    if ( streq( arg, "-h" ) || streq( arg, "--help" ) )
    {
      usage();
      exit( EXIT_SUCCESS );
    }
    else if ( streq( arg, "-b" ) || streq( arg, "--bell" ) )
    {
      enableBell = true;
    }
    else if ( streq( arg, "-s" ) || streq( arg, "--suspend" ) )
    {
      doSuspend = true;
    }
    else if ( streq( arg, "-v" ) || streq( arg, "--verbose" ) )
    {
      if ( Logger::LL_Verbose < logLevel )
        logLevel = Logger::LL_Verbose;
    }
    else if ( streq( arg, "--debug" ) )
    {
      logLevel = Logger::LL_Debug;
    }
    else if ( streq( arg, "--logfile" ) )
    {
      logfile = *(++argv);
      if ( ! logfile )
      {
        usage();
        errx( EXIT_FAILURE, "no logfile specified\n" );
      }
    }
    else if ( streq( arg, "--version" ) )
    {
      std::cout <<
        "slock2\n"
        "Version " VERSION "\n"
        __DATE__ "\n"
        "Immanuel Haffner\n"
        ;
      exit( EXIT_SUCCESS );
    }
    else
    {
      /* Unknown argument. */
      std::cerr << "unknown argument: '" << arg << "'" << std::endl;
      usage();
      exit( EXIT_FAILURE );
    }
  }
}
