#ifndef LOGGER_H
#define LOGGER_H


#include <cstdio>
#include <cassert>
#include <iostream>
#include <time.h>
#include "util.h"


/**
 * A simple logger to write to a logfile.
 * If the logfile could not be opened for append, logs to stdout.
 */
struct Logger
{
	enum LogLevel
	{
		VERBOSE,
		NORMAL,
		WARNING,
		ERROR
	};

	/**
	 * Creates a new Logger, that writes to the file specified by `filename`.  If
	 * the file does not exist, writes to stdout.
	 * The log level describes what kind of levels to log.
	 *
	 * @param fileanme the name of the logfile
	 * @param logLevel the log level
	 */
	static void create( char const * filename, LogLevel const logLevel = NORMAL );

	static inline Logger * get()
	{
		return Logger::instance;
	}

  ~Logger()
  {
    if ( fclose( f ) )
      std::cerr << "Warning: could not close '" << filename << "'";
  }

	/**
	 * Prints a timestamp to the log file.
	 */
	void timestamp() const;

	/**
	 * Writes a list of messages to the log file and gives it a timestamp.
	 */
  template < class... Args >
    inline void log( Args... args ) const
		{
			log( NORMAL, args... );
		}

	/**
	 * Writes a list of messages to the log file and gives it a timestamp.
	 */
  template < class... Args >
    void log( LogLevel const logLevel, Args... args ) const
    {
			if ( logLevel < this->logLevel )
				return;

			FILE *out = f ? f : stdout;

			timestamp();
			switch ( logLevel )
			{
				case ERROR:
					fprintf( out, "ERROR: " );
					break;

				case WARNING:
					fprintf( out, "WARNING: " );
					break;

				case VERBOSE:
				case NORMAL:
				default:;
			}
      _log( args... );
			fprintf( out, "\n" );
    }

  private:
	static Logger *instance;

  char const * const filename;
  FILE *f;
	LogLevel logLevel;


	Logger( char const * const filename, FILE *const f, LogLevel const logLevel )
		: filename(filename), f(f), logLevel(logLevel)
	{}

	/**
	 * Writes a message to the log file.
	 */
  void _log( char * const msg ) const
	{
		_log( const_cast< char const * >( msg ) );
	}

	/**
	 * Writes a message to the log file.
	 */
  void _log( char const * const msg ) const
  {
		FILE *out = f ? f : stdout;

    assert( msg && "message must not be NULL" );
		fprintf( out, "%s", msg );
  }

	/**
	 * Writes a list of messages to the log file.
	 */
  template < class T, class... Args >
    void _log( T t, Args... args ) const
    {
      _log( t );
      _log( args... );
    }
};


#endif
