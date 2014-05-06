#pragma once

#include <Grid/Grid.h>

#include <unistd.h>

class Game {
public:
  Game() = delete;
  Game( int fd1, int fd2 ) :
    p_one_fd( fd1 ), p_two_fd( fd2 )
  {}
  ~Game() {
    close( p_one_fd );
    close( p_two_fd );
  }

  void run();

private:
  int p_one_fd, p_two_fd;
  Grid grid;
};