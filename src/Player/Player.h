#pragma once

#include <Client/Client.h>
#include <Grid/Grid.h>

class Player :
  public Client {
public:
  Player();

  void play();

private:
  Grid grid;
};