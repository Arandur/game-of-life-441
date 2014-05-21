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
  puts( "Game sent state to players" );
#endif  // DEBUG

  while( running ) {
    getCommand( p_one_fd )();
    getCommand( p_two_fd )();

    if( p_one_move and p_two_move ) {
      grid.setCell( fromJust( p_one_move ), PlayerNumber::ONE );
      grid.setCell( fromJust( p_two_move ), PlayerNumber::TWO );

      grid.tick();

      if( game_over() ) {
        end();
        running = false;
      } else {
        send_state( p_one_fd );
        send_state( p_two_fd );

        p_one_move = nullptr;
        p_two_move = nullptr;
      }
    }
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

std::function< void() > Game::getCommand( int fd ) {
  int rv = recv( fd, buffer, 256, 0 );

  if( rv == 0 ) {
#ifdef DEBUG
    printf( "Game lost connection from player %d\n",
            ( fd == p_one_fd ) ? 1 : 2 );
#endif  // DEBUG
    return std::bind( &Game::end, this );
  } else if( rv < 0 ) {
    perror( "recv" );
    return std::bind( &Game::end, this );
  } else {
    if( is_forfeit( buffer ) ) {
#ifdef DEBUG
      printf( "Game received forfeit from player %d\n",
              ( fd == p_one_fd ) ? 1 : 2 );
#endif  // DEBUG
      return std::bind( &Game::end, this );
    } else if( is_move( buffer ) ) {
#ifdef DEBUG
      printf( "Game received move from player %d\n",
              ( fd == p_one_fd ) ? 1 : 2 );
      printf( "Game: " );
      print_move( buffer );
#endif  // DEBUG
      if( fd == p_one_fd )
        p_one_move = Just( get_move( buffer ) );
      else
        p_two_move = Just( get_move( buffer ) );
      return [] {};
    } else if( is_query( buffer ) ) {
#ifdef DEBUG
      printf( "Game received query from player %d\n",
              ( fd == p_one_fd ) ? 1 : 2 );
#endif  // DEBUG
      return std::bind( &Game::send_state, this, fd );
    }
  }

#ifdef DEBUG
  printf( "Game received bad command from player %d\n",
              ( fd == p_one_fd ) ? 1 : 2 );
  for( char c : buffer ) {
    if( c < 3 )
      printf( "%d", static_cast< int >( c ) );
    else
      putchar( c );
  }
    puts( "" );
#endif  // DEBUG

  return [] {};
}

bool Game::game_over() const {
  return grid.count_cells( PlayerNumber::ONE ) == 0 or
         grid.count_cells( PlayerNumber::TWO ) == 0;
}

void Game::send_state( int fd ) {
  make_grid( buffer, grid );
#ifdef DEBUG
  puts( "Sending state..." );
  printf( "Game: " );
  print_grid( buffer );
#endif  // DEBUG
  send( fd, buffer, 256, 0 );
}

void Game::end() {
  make_end( buffer );
  send( p_one_fd, buffer, 256, 0 );
  send( p_two_fd, buffer, 256, 0 );
}
