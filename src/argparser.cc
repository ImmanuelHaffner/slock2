#include "argparser.h"

#include <cstddef>
#include <cstdlib>
#include <err.h>
#include <iostream>
#include "util.h"


void usage()
{
  std::cout <<
    "slock\n"
    "  -h  --help             - prints this message\n"
    "  -v  --verbose          - make slock verbose\n"
    "  -l  --logfile <FILE>    - sets the log file\n"
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
    else if ( streq( arg, "-v" ) || streq( arg, "--verbose" ) )
    {
      verbose = true;
    }
    else if ( streq( arg, "-l" ) || streq( arg, "--logfile" ) )
    {
      logfile = *(++argv);
      if ( ! logfile )
      {
        usage();
        errx( EXIT_FAILURE, "no logfile specified\n" );
      }
    }
    else
    {
      /* Unknown argument. */
      std::cerr << "unknown argument: '" << arg << "'" << std::endl;
      exit( EXIT_FAILURE );
    }
  }
}
