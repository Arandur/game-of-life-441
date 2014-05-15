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
#ifdef DEBUG
    puts( "Play!" );
#endif  // DEBUG
    GridCoordinates gc;
    Maybe< GridCoordinates > mgc;
    bool game = true;

    connect();
    puts( "Connected!" );

    while( game ) {
      receive( msg );
#ifdef DEBUG
      puts( "Received!" );
#endif  // DEBUG

      if( is_grid( msg ) ) {
        // This should be changed to effect a transition animation.
#ifdef DEBUG
        puts ( "Grid!" );
#endif  // DEBUG
        grid = get_grid( msg );
      } else if( is_end( msg ) ) {
#ifdef DEBUG
        puts( "Game should be ending!" );
#endif  // DEBUG
        game = false;
      } else throw std::runtime_error( msg );  // Just for now
      
      mgc = getMove();
      if( mgc ) {
        gc = fromJust( mgc );
        make_move( msg, gc );
        send( msg );
      } else {
#ifdef DEBUG
        puts( "Player sent forfeit!" );
#endif  // DEBUG
        forfeit();
        game = false;
      }
    }

#ifdef DEBUG
    puts( "Player::play is done!" );
#endif  // DEBUG
  }

private:
  void forfeit() {
    make_forfeit( msg );
    send( msg );
  }

  char msg[256];
};
