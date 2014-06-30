#include "event.h"

#include <cassert>


void loginSuccess()
{
}

void loginFail()
{
}


void raiseEvent( char const * const name )
{
  assert( name && "name must not be NULL" );
}
