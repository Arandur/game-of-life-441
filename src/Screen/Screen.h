#pragma once

#include <Brain/Brain.h>
#include <Grid/Grid.h>

#include "SDL2/SDL.h"

#include <cstdint>
#include <memory>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

namespace colors {
struct Color_t {
  Color_t() :
    Color_t( 0, 0, 0, 0 )
  {}

  Color_t( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :
    red( _r ), green( _g ), blue( _b ), alpha( _a )
  {}

  operator int () const {
    return
      ( (int) red   << 6 ) |
      ( (int) green << 4 ) |
      ( (int) blue  << 2 ) |
      ( (int) alpha << 0 ) ;
  }

  bool operator < ( const Color_t& other ) {
    return this->operator int() < (int)other;
  }

  uint8_t red, green, blue, alpha;
};

const Color_t WHITE{ 255, 255, 255,   0 };
const Color_t RED  { 255,   0,   0,   0 };
const Color_t GREEN{   0, 255,   0,   0 };
const Color_t BLUE {   0,   0, 255,   0 };
const Color_t GRAY { 100, 100, 100,   0 };
const Color_t BLACK{   0,   0,   0,   0 };
const Color_t CLEAR{   0,   0,   0, 255 };
}

class Screen :
  public Brain {
public:
  Screen();
  ~Screen();

  virtual Maybe< GridCoordinates > getMove();

protected:
  Grid grid;

private:
  std::unique_ptr< SDL_Window, void(*)( SDL_Window* ) > window;
  std::unique_ptr< SDL_Renderer, void(*)( SDL_Renderer* ) > renderer;

  colors::Color_t color_map[3] = { colors::GRAY,
                                   colors::BLUE,
                                   colors::RED };

  static SDL_Rect makeRectFromGC( uint8_t, uint8_t );
  static GridCoordinates GCfromSC( int, int );
  void setRenderDrawColor( colors::Color_t );

  void render();
  void render_grid();
};
