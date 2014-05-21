#pragma once

#include <Coordinates/Coordinates.h>
#include <Grid/Grid.h>

#include <cstring>
#include <functional>
#include <cstdint>

Grid get_grid( char (&)[256] );

GridCoordinates get_move( char (&)[256] );

void make_query( char (&)[256] );

void make_move( char (&)[256], const GridCoordinates& );

void make_grid( char (&)[256], const Grid& );

void make_forfeit( char (&)[256] );

void make_end( char (&)[256] );

bool is_grid( char (&)[256] );

bool is_query( char (&)[256] );

bool is_move( char (&)[256] );

bool is_forfeit( char (&)[256] );

bool is_end( char (&)[256] );

#ifdef DEBUG
void print_grid( char (&)[256] );

void print_move( char (&)[256] );
#endif  // DEBUG
