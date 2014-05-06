#pragma once

#include <Game/Game.h>

class Server {
public:
  Server();

  void run();

private:
  int sockfd;
};