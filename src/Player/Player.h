#pragma once

#include <Client/Client.h>
#include <Grid/Grid.h>
#include <Brain/Brain.h>
#include <util/Maybe.h>

#include <type_traits>

#include <iostream>

template < class B >
class Player :
  public Client, public std::enable_if< std::is_base_of< Brain, B >::value, B >::type {
public:
  using B::getMove;

  Player< B >() :
    B( grid ) {}

  void play() {
    GridCoordinates gc;
    Maybe< GridCoordinates > mgc = getMove();
    if( mgc ) {
      gc = fromJust( mgc );
      std::cout << (int)gc.x << ", " << (int)gc.y << std::endl;
    } else forfeit();
  }

protected:
  Grid grid;

  void forfeit() {

  }
};