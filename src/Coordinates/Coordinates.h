#pragma once

#include <type_traits>

template < class T >
struct Coordinates {
  Coordinates() = default;
  Coordinates( T& _x, T& _y ) :
    x( _x ), y( _y )
  {}

  T x = T();
  T y = T();

  static Coordinates< T& > tie( T& _x, T& _y ) {
    return { _x, _y };
  }

  Coordinates< T >& operator = (
    const Coordinates< typename std::remove_reference< T >::type >& c ) {
    x = c.x;
    y = c.y;
    return *this;
  }
};

struct ScreenCoordinates :
  public Coordinates< int > {
  ScreenCoordinates() = default;
  ScreenCoordinates( int _x, int _y ) :
    Coordinates( _x, _y )
  {}
};

struct GridCoordinates :
  public Coordinates< uint8_t > {
  GridCoordinates() = default;
  GridCoordinates( uint8_t _x, uint8_t _y ) :
    Coordinates( _x, _y )
  {}
};