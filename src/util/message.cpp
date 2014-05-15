#include "./message.h"

bool has_prefix( char (&arr)[256], const char* str ) {
  char* p = &arr[0];
  while( *str )
    if( *p++ != *str++ )
      return false;
  return true;
}

Grid get_grid( char (&arr)[256] ) {
  char grid[ GRID_SIZE + 1 ];
  memcpy( grid, &arr[4], GRID_SIZE + 1 );
  return Grid( grid );
}

GridCoordinates get_move( char (&arr)[256] ) {
  return { static_cast< uint8_t >( arr[4] ),
           static_cast< uint8_t >( arr[5] ) };
}

void make_query( char (&arr)[256] ) {
  memcpy( &arr[0], "Query", 5 );
}

void make_move( char (&arr)[256], const GridCoordinates& gc ) {
  memcpy( &arr[0], "Move", 4 );
  arr[4] = gc.x;
  arr[5] = gc.y;
}

void make_grid( char (&arr)[256], const Grid& grid ) {
  memcpy( &arr[0], "Grid", 4 );
  memcpy( &arr[4], grid.get_string_representation(), GRID_SIZE );
}

void make_forfeit( char (&arr)[256] ) {
  memcpy( &arr[0], "Forfeit", 7 );
}

void make_end( char (&arr)[256] ) {
  memcpy( &arr[0], "End", 3 );
}

bool is_grid( char (&arr)[256] ) {
  return has_prefix( arr, "Grid" );
}

bool is_query( char (&arr)[256] ) {
  return has_prefix( arr, "Query" );
}

bool is_move( char (&arr)[256] ) {
  return has_prefix( arr, "Move" );
}

bool is_forfeit( char (&arr)[256] ) {
  return has_prefix( arr, "Forfeit" );
}

bool is_end( char (&arr)[256] ) {
  return has_prefix( arr, "End" );
}
