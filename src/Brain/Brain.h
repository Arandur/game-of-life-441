#pragma once

#include <Coordinates/Coordinates.h>
#include <util/Maybe.h>

class Brain {
public:
  virtual Maybe< GridCoordinates > getMove() = 0;
};