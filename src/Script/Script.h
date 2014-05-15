#pragma once

#include <Brain/Brain.h>
#include <Grid/Grid.h>

class Script :
  public Brain {
public:
  Script() = default;

  Maybe< GridCoordinates > getMove();

protected:
  Grid grid;
};