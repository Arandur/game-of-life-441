#pragma once

#include <Grid/Grid.h>
#include <Coordinates/Coordinates.h>
#include <util/Maybe.h>

class Brain {
public:
  Brain() = default;
  virtual ~Brain() = default;

  virtual Maybe< GridCoordinates > getMove() = 0;

protected:
  Grid grid;
};