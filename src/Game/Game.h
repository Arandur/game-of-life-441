#pragma once

#include <Grid/Grid.h>
#include <util/message.h>

#include <unistd.h>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

class Game {
public:
  Game() = delete;
  Game( int fd1, int fd2 ) :
    p_one_fd( fd1 ), p_two_fd( fd2 )//,
    //p_one_handler( &Game::watch, this, p_one_fd ),
    //p_two_handler( &Game::watch, this, p_two_fd )
  {
    //p_one_handler.detach();
    //p_two_handler.detach();

    grid_init();
  }
  ~Game() {
    close( p_one_fd );
    close( p_two_fd );
  }

  void run();

private:
  int p_one_fd, p_two_fd;
  Grid grid;
  std::mutex jobs_mutex, buffer_mutex;
  std::queue< std::function< void() > > jobs;
  char buffer[256];
  std::thread p_one_handler, p_two_handler;
  bool running = true;

  void grid_init();
  void watch( int );
  void send_state( int );
  void end();
};
