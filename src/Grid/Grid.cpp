#include "./Grid.h"

#include <algorithm>
#include <numeric>
#include <cassert>
#include <iterator>
#include <stdexcept>

Grid::Grid() {
  for( int i = 0; i < GRID_HEIGHT; ++i )
  for( int j = 0; j < GRID_WIDTH;  ++j ) {
    cells_a[i][j] = 0;
  }
}

Grid::Grid( const char (&str) [ GRID_HEIGHT * GRID_WIDTH + 1 ] ) {
  uint8_t count_zeroes = 0;
  for( int i = 0; i < GRID_HEIGHT; ++i )
  for( int j = 0; j < GRID_WIDTH;  ++j ) {
    if( ( cells_a[i][j] = str[ i * GRID_WIDTH + j + 1 ] ) == 0 )
      ++count_zeroes;
  }

  if( count_zeroes != str[0] )
    throw std::runtime_error( "Invalid Grid" );
}

void Grid::setCell( GridCoordinates gc, PlayerNumber p ) {
#ifdef DEBUG
  assert( 0 <= x and x < GRID_HEIGHT and
          0 <= y and y < GRID_WIDTH );
#endif  // DEBUG
  uint8_t& cell = const_cast< uint8_t(*)[GRID_HEIGHT] >( grid )[ gc.x ][ gc.y ];
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
        ++neighbors[ grid[ x + i ][ y + j ] ];
      else
        ++neighbors[0];

#ifdef DEBUG
    assert( std::accumulate( std::begin( neighbors ),
            std::end( neighbors ),
            0 ) == 8 );
#endif  // DEBUG

    // Calculate change
    if( ( grid[x][y] == 0 and neighbors[0] == 5 ) or
        neighbors[0] == 5 or neighbors[0] == 6 ) {
      auto it = std::max_element( std::next( std::begin( neighbors ) ),
                                  std::end( neighbors ) );
      if( *it == 1 )
        grid_reserve[x][y] = grid[x][y];
      else
        grid_reserve[x][y] = it - std::begin( neighbors );
    } else grid_reserve[x][y] = 0;
  }

  flip();
}

const char* Grid::get_string_representation() const {
  char* str = new char[ GRID_HEIGHT * GRID_WIDTH + 1 ];
  uint8_t count_zeroes = 0;

  for( int i = 0; i < GRID_HEIGHT; ++i )
  for( int j = 0; j < GRID_WIDTH;  ++j ) {
    if( ( str[ i * GRID_WIDTH + j + 1 ] = grid[i][j] ) == 0 )
      ++count_zeroes;
  }

  str[0] = count_zeroes;

  return str;
}

bool Grid::operator == ( const Grid& g ) const {
  for( int i = 0; i < GRID_HEIGHT; ++i )
  for( int j = 0; j < GRID_WIDTH;  ++j ) {
    if( grid[i][j] != g.grid[i][j] )
      return false;
  }

  return true;
}

void Grid::flip() {
  std::swap( const_cast< uint8_t(*&)[GRID_HEIGHT] >( grid ), grid_reserve );
}