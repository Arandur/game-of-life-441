#include "./Screen.h"

#include <util/make_resource.h>

#include <cassert>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>

#define RECT_BORDER 1

template < typename... Arguments >
auto make_window( Arguments&&... args ) {
  return make_resource( SDL_CreateWindow, SDL_DestroyWindow,
                        std::forward< Arguments >( args )... );
}

template < typename... Arguments >
auto make_renderer( Arguments&&... args ) {
  return make_resource( SDL_CreateRenderer, SDL_DestroyRenderer,
                        std::forward< Arguments >( args )... );
}

Screen::Screen( Grid& g, void (&q)() ) try :
  Brain( g, q ),
  window( make_window( "Game of Life", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
                       SDL_WINDOW_SHOWN ) ),
  renderer( make_renderer( window.get(), -1,
                           SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_PRESENTVSYNC ) ) {
  if( !window )
    throw std::runtime_error( "Could not create window" );

  if( !renderer )
    throw std::runtime_error( "Could not create renderer" );
} catch( std::exception& e ) {
  SDL_Quit();
  throw e;
}

Screen::~Screen() {
  SDL_Quit();
}

Maybe< GridCoordinates > Screen::getMove() {
  render();

  SDL_Event event;

  while( true )
  while( SDL_PollEvent( &event ) ) {
    switch( event.type ) {
    case SDL_QUIT:
      quit_callback();
      return nullptr;
      break;
    case SDL_MOUSEBUTTONDOWN:
      return Just( GCfromSC( event.button.x, event.button.y ) );
      break;
    default:
      break;
    }
  }
}

SDL_Rect Screen::makeRectFromGC( uint8_t x, uint8_t y ) {
#ifdef DEBUG
  assert( x < GRID_WIDTH );
  assert( y < GRID_HEIGHT );
#endif  // DEBUG

  SDL_Rect rect;
  rect.x = SCREEN_WIDTH  / GRID_WIDTH  * x + RECT_BORDER;
  rect.y = SCREEN_HEIGHT / GRID_HEIGHT * y + RECT_BORDER;
  rect.w = SCREEN_WIDTH  / GRID_WIDTH  - 2 * RECT_BORDER;
  rect.h = SCREEN_HEIGHT / GRID_HEIGHT - 2 * RECT_BORDER;

  return rect;
}

GridCoordinates Screen::GCfromSC( int x, int y ) {
  return { x * GRID_WIDTH  / SCREEN_WIDTH,
           y * GRID_HEIGHT / SCREEN_HEIGHT };
}

void Screen::setRenderDrawColor( colors::Color_t color ) {
  if( SDL_SetRenderDrawColor( renderer.get(), color.red,
                                              color.green,
                                              color.blue,
                                              color.alpha ) != 0 )
    throw std::runtime_error( "Could not set render draw color" );
}

void Screen::render() {
  setRenderDrawColor( colors::WHITE );
  SDL_RenderFillRect( renderer.get(), nullptr );

  render_grid();
}

void Screen::render_grid() {
  SDL_Rect rect;
  for( uint8_t x = 0; x < GRID_HEIGHT; ++x )
  for( uint8_t y = 0; y < GRID_WIDTH ; ++y ) {
    setRenderDrawColor( color_map[ grid.grid[x][y] ] );
    rect = makeRectFromGC( x, y );
    SDL_RenderFillRect( renderer.get(), &rect );
  }
}