#pragma once

#include <Player/Player.h>
#include <Screen/Screen.h>

class HumanPlayer :
  public Player< Screen > {
public:
  HumanPlayer() :
    Player()
  {}

private:

};