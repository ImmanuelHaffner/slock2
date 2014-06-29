#include "argparser.h"

#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <err.h>
#include <iostream>


inline bool streq( char const * const s0, char const * const s1 )
{
  return strcmp( s0, s1 ) == 0;
}

void usage()
{
  std::cout <<
    "slock\n"
    "  -h  --help             - prints this message\n"
    "  -v  --verbose          - make slock verbose\n"
		"  -l  --logfile <FILE>		- sets the log file\n"
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
