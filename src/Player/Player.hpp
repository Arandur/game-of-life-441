#pragma once

#include <Client/Client.h>
#include <Grid/Grid.h>
#include <Brain/Brain.h>
#include <util/Maybe.h>
#include <util/message.h>

#include <type_traits>
#include <algorithm>
#include <cstdio>

template < class B >
class Player :
  public Client,
  public std::enable_if< std::is_base_of< Brain, B >::value, B >::type {
public:
  using B::getMove;
  using B::grid;

  Player< B >() = default;

  void play() {
    GridCoordinates gc;
    Maybe< GridCoordinates > mgc;
    bool game = true;

    connect();
#ifdef DEBUG
    puts( "Connected!" );
#endif  // DEBUG

    while( game ) {
      receive( msg );
#ifdef DEBUG
      puts( "Received message from game" );
#endif  // DEBUG

      if( is_grid( msg ) ) {
        // This should be changed to effect a transition animation.
#ifdef DEBUG
        puts( "Message is grid" );
        printf( "Player: " );
        print_grid( msg );
#endif  // DEBUG
        grid = get_grid( msg );
      } else if( is_end( msg ) ) {
#ifdef DEBUG
        puts( "Message is end" );
#endif  // DEBUG
        game = false;
      } else throw std::runtime_error( msg );  // Just for now
      
      mgc = getMove();
      if( mgc ) {
        gc = fromJust( mgc );
        make_move( msg, gc );
#ifdef DEBUG
        printf( "Player: ( %d, %d )\n", gc.x, gc.y );
#endif  // DEBUG
        send( msg );
      } else {
#ifdef DEBUG
        puts( "Player is sending forfeit..." );
#endif  // DEBUG
        forfeit();
        game = false;
      }
    }
  }

private:
  void forfeit() {
    make_forfeit( msg );
    send( msg );
  }

  char msg[256];
};
