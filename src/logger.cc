#include "logger.h"


#include "util.h"


/**
 * Creates a new Logger, that writes to the file specified by `filename`.
 * The log level describes what kind of levels to log.
 *
 * On success, replaces the current logger instance with the newly created
 * logger.  Otherwise, returns NULL.
 *
 * @param fileanme the name of the logfile
 * @param logLevel the log level
 * @return the newly created logger, or NULL on failure
 */
Logger * Logger::create( char const * filename,
    LogLevel const logLevel /* = NORMAL */ )
{
  Logger *logger = NULL;

  if ( ! filename )
    logger = new Logger( "<stdout>", stdout, logLevel );
  else
  {
    FILE *out = fopen( filename, "a" );
    if ( ! out )
      return NULL;
    logger = new Logger( filename, out, logLevel );
  }

  delete Logger::instance;
  Logger::instance = logger;
  return Logger::instance;
}

Logger * Logger::create( LogLevel const logLevel /* = NORMAL */ )
{
  return create( NULL, logLevel );
}

/**
 * Prints a timestamp to the log file.
 */
void Logger::timestamp() const
{
  time_t thetime = time( NULL );
  struct tm *date = localtime( &thetime );

  fprintf( f, "%4d-%02d-%02d %02d:%02d:%02d - ",
      date->tm_year + 1900,   /* year */
      date->tm_mon + 1,       /* month */
      date->tm_mday,          /* day of the month */
      date->tm_hour,          /* hour */
      date->tm_min,           /* minutes */
      date->tm_sec            /* seconds */ );
  fflush( f );
}

Logger *Logger::instance = NULL;
