#ifndef UTIL_H
#define UTIL_H


#include <cstring>


template < class T >
T * notnull( T *v )
{
	assert( v && "v must not be NULL" );
	return v;
}

inline bool streq( char const * const s0, char const * const s1 )
{
  return strcmp( s0, s1 ) == 0;
}


#endif
