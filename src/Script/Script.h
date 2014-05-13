#pragma once

#include <Brain/Brain.h>

class Script :
  public Brain {
public:
  Script() = default;

  Maybe< GridCoordinates > getMove();
};