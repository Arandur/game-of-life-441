#include "./Script.h"

Maybe< GridCoordinates > Script::getMove() {
  return Just< GridCoordinates >( { 0, 0 } );
}