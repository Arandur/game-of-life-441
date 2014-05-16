#include "./Grid.h"

#include <algorithm>
#include <numeric>
#include <cassert>
#include <iterator>
#include <stdexcept>

Grid::Grid() :
  grid( cells_a ),
  grid_reserve( cells_b ) {
  std::fill( std::begin( cells_a ), std::end( cells_a ), 0 );
}

Grid::Grid( const char (&str) [ GRID_SIZE + 1 ] ) :
  grid( cells_a ),
  grid_reserve( cells_b ) {
  uint8_t n_zeroes = std::count_if( std::begin( str ), std::end( str ), [] ( const char& c ) -> bool {
    return c == 0;
  } );

  if( n_zeroes != str[0] ) {
    puts( "Invalid grid..." );
  }

  std::copy_n( &str[1], GRID_SIZE, std::begin( grid ) );
}

Grid& Grid::operator = ( const Grid& g ) {
  cells_a = g.cells_a;
  cells_b = g.cells_b;
  if( g.grid == g.cells_a ) {
    grid = cells_a;
    grid_reserve = cells_b;
  } else {
    grid = cells_b;
    grid_reserve = cells_a;
  }

  return *this;
}

Grid& Grid::operator = ( const char (&str) [ GRID_SIZE + 1 ] ) {
  uint8_t n_zeroes = std::count_if( std::begin( str ), std::end( str ),
                                    [] ( const char& c ) -> bool {
    return c == 0;
  } );

  if( n_zeroes != str[0] )
    throw std::runtime_error( "Invalid Grid" );

  std::copy_n( &str[1], GRID_SIZE, std::begin( grid ) );

  return *this;
}

void Grid::setCell( GridCoordinates gc, PlayerNumber p ) {
#ifdef DEBUG
  assert( gc.x < GRID_HEIGHT and
          gc.y < GRID_WIDTH );
#endif  // DEBUG
  uint8_t& cell = grid[ GRID_WIDTH * gc.x + gc.y ];
  switch( p ) {
  case PlayerNumber::ONE:
    cell = 1;
    break;
  case PlayerNumber::TWO:
    cell = 2;
    break;
  }
}

void Grid::tick() {
  // This needs to be placed in a Lua script, but let's do it in C++ for now.
  int neighbors[3] = { 0, 0, 0 };

  for( int x = 0; x < GRID_HEIGHT; ++x )
  for( int y = 0; y < GRID_WIDTH;  ++y ) {

    // Get neighbors
    std::fill( std::begin( neighbors ), std::end( neighbors ), 0 );
    for( int i : { -1, 0, 1 } )
    for( int j : { -1, 0, 1 } )
      if( i == 0 and j == 0 )
        continue;
      else if( 0 <= x + i and x + i < GRID_HEIGHT and
               0 <= y + j and y + j < GRID_WIDTH )
        ++neighbors[ (*this)[ x + i ][ y + j ] ];
      else
        ++neighbors[0];

#ifdef DEBUG
    assert( std::accumulate( std::begin( neighbors ),
            std::end( neighbors ),
            0 ) == 8 );
#endif  // DEBUG

    // Calculate change
    if( ( (*this)[x][y] == 0 and neighbors[0] == 5 ) or
        neighbors[0] == 5 or neighbors[0] == 6 ) {
      auto it = std::max_element( std::next( std::begin( neighbors ) ),
                                  std::end( neighbors ) );
      if( *it == 1 )
        grid_reserve[ GRID_WIDTH * x + y ] = grid[ GRID_WIDTH * x + y ];
      else
        grid_reserve[ GRID_WIDTH * x + y ] = it - std::begin( neighbors );
    } else grid_reserve[ GRID_WIDTH * x + y ] = 0;
  }

  flip();
}

const char* Grid::get_string_representation() const {
  static char str[ GRID_SIZE + 1 ];
  uint8_t n_zeroes = std::count_if( std::begin( grid ), std::end( grid ), [] ( uint8_t i ) -> bool {
    return i == 0;
  } );

  std::copy_n( std::begin( grid ), GRID_SIZE, std::begin( str ) + 1 );

  str[0] = n_zeroes;

  return str;
}

bool Grid::operator == ( const Grid& g ) const {
  return grid == g.grid;
}

const uint8_t* Grid::operator [] ( uint8_t i ) const {
  static uint8_t row[GRID_WIDTH];
  std::copy_n( std::begin( grid ) + GRID_WIDTH * i, GRID_WIDTH, std::begin( row ) );
  return row;
}

void Grid::flip() {
  std::swap( grid, grid_reserve );
}
