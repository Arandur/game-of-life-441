#pragma once

#include <Grid/Grid.h>
#include <Coordinates/Coordinates.h>
#include <util/Maybe.h>

#include <functional>

class Brain {
public:
  Brain( Grid& g ) :
    grid( g )
  {}
  virtual ~Brain() = default;

  virtual Maybe< GridCoordinates > getMove() = 0;

protected:
  Grid& grid;
};