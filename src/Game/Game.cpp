#include "./Game.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>

#include <vector>
#include <algorithm>
#include <random>

#define N_STARTING_CELLS 10

void Game::run() {
  send_state( p_one_fd );
  send_state( p_two_fd );
  while( running ) {
    jobs_mutex.lock();
    if( ! jobs.empty() ) {
      jobs.front()();
      jobs.pop();
    }
    jobs_mutex.unlock();
  }
}

// This is a candidate for Lua porting; it should perhaps be part of the
//   World script.
void Game::grid_init() {
  std::vector< uint8_t > vec( GRID_SIZE / 4, 0 );
  std::iota( std::begin( vec ), std::end( vec ), 0 );
  std::random_shuffle( std::begin( vec ), std::end( vec ) );
  vec.resize( N_STARTING_CELLS );

  uint8_t x, y;

  for( uint8_t i : vec ) {
    x = 2 * i / GRID_WIDTH;
    y = GRID_HEIGHT / 2 + i % ( GRID_HEIGHT / 2 );
    grid.setCell( { x, y },
                  PlayerNumber::ONE );
    grid.setCell( { static_cast< uint8_t >( GRID_WIDTH  - 1 - x ),
                    static_cast< uint8_t >( GRID_HEIGHT - 1 - y ) },
                  PlayerNumber::TWO );
  }
}

void Game::watch( int fd ) {
  int rv;

  while( true ) {
    buffer_mutex.lock();
    rv = recv( fd, buffer, 256, 0 );
    if( rv == 0 ) {
      jobs_mutex.lock();
      jobs.push( std::bind( &Game::end, this ) );
      jobs_mutex.unlock();
    } else if( rv < 0 ) {
      perror( "recv" );
    } else {
      
    }
    buffer_mutex.unlock();
  }
}

void Game::send_state( int fd ) {
  send( fd, grid.get_string_representation(), GRID_SIZE + 1, 0 );
}

void Game::end() {
  send( p_one_fd, "End", 3, 0 );
  send( p_two_fd, "End", 3, 0 );
}
