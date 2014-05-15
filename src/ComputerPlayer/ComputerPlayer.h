#pragma once

#include <Player/Player.hpp>
#include <Script/Script.h>

class ComputerPlayer :
  public Player< Script > {
public:
  ComputerPlayer() :
    Player()
  {}
};
