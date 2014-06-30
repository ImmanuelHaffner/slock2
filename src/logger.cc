#include "logger.h"


#include "util.h"


/**
 * Creates a new Logger, that writes to the file specified by `filename`.  If
 * the file does not exist, writes to stdout.
 * The log level describes what kind of levels to log.
 *
 * @param fileanme the name of the logfile
 * @param logLevel the log level
 */
void Logger::create( char const * filename,
    LogLevel const logLevel /* = NORMAL */ )
{
  if ( Logger::instance )
    delete Logger::instance;

  FILE *out = NULL;

  if ( filename )
    out = fopen( filename, "a" );

  Logger::instance = new Logger( filename, out ? out : stdout, logLevel );
  if ( ! out && filename )
    Logger::instance->w( "could not open '", filename, "' for logging" );
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
}

Logger *Logger::instance = NULL;
