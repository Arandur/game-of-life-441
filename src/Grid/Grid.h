#pragma once

#include <cstdint>
#include <array>

#include <Coordinates/Coordinates.h>

#define GRID_HEIGHT 8
#define GRID_WIDTH  8
#define GRID_SIZE   ( GRID_HEIGHT * GRID_WIDTH )

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

  bool operator == ( const Grid& ) const;

  const uint8_t* operator [] ( uint8_t ) const;

private:
  std::array< uint8_t, GRID_SIZE >& grid = cells_a;
  std::array< uint8_t, GRID_SIZE >& grid_reserve = cells_b;
  std::array< uint8_t, GRID_SIZE >  cells_a;
  std::array< uint8_t, GRID_SIZE >  cells_b;

  void flip();
};