#pragma once

#include <Grid/Grid.h>
#include <Coordinates/Coordinates.h>
#include <util/Maybe.h>

class Brain {
public:
  Brain( Grid& g, void (&q)() ) :
    grid( g ), quit_callback( q )
  {}
  virtual ~Brain() = default;

  virtual Maybe< GridCoordinates > getMove() = 0;

protected:
  Grid& grid;
  void (&quit_callback)();
};