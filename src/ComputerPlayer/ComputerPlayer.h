#pragma once

#include <Player/Player.h>
#include <Script/Script.h>

class ComputerPlayer :
  public Player< Script > {
public:
  ComputerPlayer() :
    Player()
  {}

private:

};