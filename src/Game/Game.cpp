#include "./Game.h"
#include <util/message.h>

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
#ifdef DEBUG
  puts( "Game run!" );
#endif  // DEBUG
  send_state( p_one_fd );
  send_state( p_two_fd );
#ifdef DEBUG
  puts( "Sent state!" );
#endif  // DEBUG
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
  bool get = false;

  while( true ) {
    jobs_mutex.lock();
    get = jobs.empty();
    jobs_mutex.unlock();

    if( get ) {
      buffer_mutex.lock();
      rv = recv( fd, buffer, 256, 0 );
      if( rv == 0 ) {
        jobs_mutex.lock();
        jobs.push( std::bind( &Game::end, this ) );
        jobs_mutex.unlock();
      } else if( rv < 0 ) {
        perror( "recv" );
      } else {
        if( is_forfeit( buffer ) ) {
#ifdef DEBUG
          puts( "Received forfeit!" );
#endif  // DEBUG
          jobs_mutex.lock();
          jobs.push( std::bind( &Game::end, this ) );
          jobs_mutex.unlock();
        } else if( is_move( buffer ) ) {
          // Register move
        } else if( is_query( buffer ) ) {
          jobs_mutex.lock();
          jobs.push( std::bind( &Game::send_state, this, fd ) );
          jobs_mutex.unlock();
        }
      }
      buffer_mutex.unlock();
    }
  }
}

void Game::send_state( int fd ) {
  buffer_mutex.lock();
  make_grid( buffer, grid );
  send( fd, buffer, 256, 0 );
  buffer_mutex.unlock();
}

void Game::end() {
  send( p_one_fd, "End", 3, 0 );
  send( p_two_fd, "End", 3, 0 );
}
