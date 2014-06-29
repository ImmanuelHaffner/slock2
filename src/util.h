#ifndef UTIL_H
#define UTIL_H


template < class T >
T * notnull( T *v )
{
	assert( v && "v must not be NULL" );
	return v;
}


#endif
