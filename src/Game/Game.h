#pragma once

#include <Grid/Grid.h>
#include <util/message.h>
#include <util/Maybe.h>
#include <Coordinates/Coordinates.h>

#include <unistd.h>
#include <functional>

class Game {
public:
  Game() = delete;
  Game( int fd1, int fd2 ) :
    p_one_fd( fd1 ), p_two_fd( fd2 )
  {
    grid_init();
  }
  ~Game() {
    close( p_one_fd );
    close( p_two_fd );
  }

  void run();

private:
  int p_one_fd, p_two_fd;
  Maybe< GridCoordinates > p_one_move, p_two_move;
  Grid grid;
  char buffer[256];
  bool running = true;

  void grid_init();
  std::function< void() > getCommand( int );
  void send_state( int );
  void end();
};
