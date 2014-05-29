#pragma once

#include <cstdint>

/**
 * \brief Encapsulates color
 *
 * In SDL, there is a specific format for colors. This struct serves simply to
 * wrap up the color data into a whole -- it also allows us to create color
 * constants.
 */
struct Color {
  Color() :
    Color( 0, 0, 0, 0 ) {}

  Color( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :
    red( _r ), green( _g ), blue( _b ), alpha( _a ) {}

  uint8_t red, green, blue, alpha;
};

namespace colors {
  const Color WHITE      {   0,   0,   0,   0 };
  const Color RED        { 255,   0,   0,   0 };
  const Color BLUE       {   0,   0, 255,   0 };
  const Color GRAY       { 100, 100, 100,   0 };
  const Color LIGHT_RED  { 127,   0,   0,   0 };
  const Color LIGHT_BLUE {   0,   0, 127,   0 };
  const Color LIGHT_GRAY {  50,  50,  50,   0 };
  const Color BLACK      { 255, 255, 255,   0 };
}
