#pragma once

#include <cstdint>

#include <Coordinates/Coordinates.h>

#define GRID_HEIGHT 8
#define GRID_WIDTH  8

enum class PlayerNumber {
  ONE,
  TWO
};

class Grid {
public:
  Grid();
  Grid( const char(&) [ GRID_HEIGHT * GRID_WIDTH + 1 ] );

  void setCell( GridCoordinates, PlayerNumber );
  void tick();

  const char* get_string_representation() const;

  const uint8_t (*grid)[GRID_HEIGHT] = cells_a;

  bool operator == ( const Grid& ) const;

private:
  uint8_t (*grid_reserve)[GRID_HEIGHT] = cells_b;
  uint8_t cells_a[GRID_HEIGHT][GRID_WIDTH];
  uint8_t cells_b[GRID_HEIGHT][GRID_WIDTH];

  void flip();
};