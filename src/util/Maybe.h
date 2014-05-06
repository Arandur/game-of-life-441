#pragma once

#include <memory>

template < class T >
using Maybe = std::unique_ptr< T >;

template < class T >
Maybe< T > Just( const T& t ) {
  return Maybe< T >( new T( t ) );
}

template < class T >
const T& fromJust( const Maybe< T >& m ) {
  return *m;
}