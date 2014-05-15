#pragma once

#include <Player/Player.hpp>
#include <Screen/Screen.h>

class HumanPlayer :
  public Player< Screen > {
public:
  HumanPlayer() :
    Player()
  {}
};
