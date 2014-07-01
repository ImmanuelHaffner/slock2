#ifndef LOGGER_H
#define LOGGER_H


#include <cstdio>
#include <cassert>
#include <iostream>
#include <time.h>
#include "util.h"


#ifndef LOGFILE
#define LOGFILE "/var/log/slock2.log"
#endif


/**
 * A simple logger to write to a logfile.
 * If the logfile could not be opened for append, logs to stdout.
 */
struct Logger
{
  enum LogLevel
  {
    LL_Debug,
    LL_Verbose, /* cause all logging to be mirrored to stdout */
    LL_Normal,
    LL_Warning,
    LL_Error
  };


  /**
   * Creates a new Logger, that writes to the file specified by `filename`.  If
   * the file does not exist, writes to stdout.
   * The log level describes what kind of levels to log.
   *
   * @param fileanme the name of the logfile
   * @param logLevel the log level
   */
  static Logger * create( char const * filename,
      LogLevel const logLevel = LL_Normal );

  static Logger * create( LogLevel const logLevel = LL_Normal );

  static inline Logger * get()
  {
    return Logger::instance;
  }

  static inline void destroy() { delete Logger::instance; }

  ~Logger()
  {
    if ( stdout != f && fclose( f ) )
      std::cerr << "WARNING: could not close '" << filename << "'" << std::endl;
  }


  /**
   * Prints a timestamp to the log file.
   */
  void timestamp() const;

  /**
   * Writes a list of messages to the log file and gives it a timestamp.
   */
  template < class... Args >
    inline void d( Args... args ) const { log( LL_Debug, args... ); }
  template < class... Args >
    inline void l( Args... args ) const { log( LL_Normal, args... ); }
  template < class... Args >
    inline void w( Args... args ) const { log( LL_Warning, args... ); }
  template < class... Args >
    inline void e( Args... args ) const { log( LL_Error, args... ); }

  /**
   * Writes a list of messages to the log file and gives it a timestamp.
   */
  template < class... Args >
    void log( LogLevel const logLevel, Args... args ) const
    {
      /* Ignore messages of too low log level. */
      if ( logLevel < this->logLevel )
        return;

      timestamp();
      switch ( logLevel )
      {
        case LL_Error:
          _log( "ERROR: " );
          break;

        case LL_Warning:
          _log( "WARNING: " );
          break;

        case LL_Debug:
          _log( "DEBUG: " );
          break;

        default:;
      }
      _log( args... );

      _log( "\n" );
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
   * Writes a list of messages to the log file.
   */
  template < class T, class... Args >
    void _log( T t, Args... args ) const
    {
      _log( t );
      _log( args... );
    }

  template< class T >
    void _log( T msg ) const
    {
      __log( f, msg );
      fflush( f );
      if ( logLevel <= LL_Verbose && stdout != f )
      {
        __log( stdout, msg );
        fflush( stdout );
      }
    }

  /**
   * Writes a message to the log file.
   */
  void __log( FILE *out, char * const msg ) const
  {
    __log( out, const_cast< char const * >( msg ) );
  }

  /**
   * Writes a message to the log file.
   */
  void __log( FILE *out, char const * const msg ) const
  {
    assert( msg && "message must not be NULL" );
    fprintf( out, "%s", msg );
  }

  void __log( FILE *out, int i )            const { fprintf( out, "%d", i ); }
  void __log( FILE *out, unsigned u )       const { fprintf( out, "%u", u ); }
  void __log( FILE *out, long l )           const { fprintf( out, "%ld", l ); }
  void __log( FILE *out, unsigned long ul ) const { fprintf( out, "%lu", ul ); }
};


#endif
