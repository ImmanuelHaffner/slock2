#include "event.h"

#include <cassert>
#include <unistd.h>
#include <cstdio>
#include "logger.h"


void loginSuccess()
{
  raiseEvent( "loginSuccess" );
}

void loginFail()
{
  raiseEvent( "loginFail" );
}


void raiseEvent( char const * const name )
{
  assert( name && "name must not be NULL" );

  FILE *check = fopen( EVENT_HANDLER, "r" );
  if ( ! check )
  {
    Logger::get()->w( "the event-handler script '" EVENT_HANDLER \
        "' does not exist" );
    return;
  }
  fclose( check );

  pid_t child_pid = fork();
  switch ( child_pid )
  {
    case -1:
      Logger::get()->w( "Failed to fork child process for event-handler" );
      break;

    case 0:
      execl(
          "/usr/bin/bash",  /* path */
          "bash",           /* 1st argument: program name */
          EVENT_HANDLER,    /* 2nd argument: shell script path */
          name              /* 3rd argument: argument to the shell script */
          );

      Logger::get()->e( "Failed to execute event-handler: ",
          strerror( errno ) );
      break;

    default:;
  }
}
