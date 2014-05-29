#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <memory>
#include <algorithm>
#include <chrono>
#include <random>

#ifdef DEBUG
#include <cassert>
#endif  // DEBUG

#include "SDL2/SDL.h"

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include <Grid.h>

#define DEFAULT_WORLD_SCRIPT "cfg/world.lua"
#define DEFAULT_AI_SCRIPT    "cfg/ai.lua"

#define HELP_STR\
  "Usage: %s [OPTIONS]\n"\
  "\n"\
  "Options: \n"\
  "\t-w, --world: Path to world script (default " DEFAULT_WORLD_SCRIPT ")\n"\
  "\t-a, --ai: Path to ai script (default " DEFAULT_AI_SCRIPT ")\n"\

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define RECT_BORDER 1
#define GRID_WIDTH 8
#define GRID_HEIGHT 8
#define GRID_SIZE (GRID_WIDTH * GRID_HEIGHT)
#define N_STARTING_CELLS 10

struct Move {
  size_t x, y;

  bool operator == ( const Move& m ) {
    return x == m.x and y == m.y;
  }

  bool operator != ( const Move& m ) {
    return !( *this == m );
  }
};

std::default_random_engine generator;
std::uniform_int_distribution< size_t > dist( 0, GRID_SIZE - 1 );

Move random_move() {
  size_t m = dist( generator );
  return { m / GRID_WIDTH, m % GRID_WIDTH };
}

/**
 * \brief A helper function for C-style SDL resources
 * 
 * Often in libraries either written in C or designed to work with either C or
 * C++, resources are managed by function pairs -- one function to allocate the
 * resource, the other to deallocate it. This function encapsulates these
 * functions for the sake of memory-safety.
 * 
 * \return A std::unique_ptr to the resource.
 */
template < typename Creator, typename Destructor, typename... Args >
auto make_resource( Creator c, Destructor d, Args&&... args ) {
  typedef typename std::decay< decltype(
      *c( std::forward< Args >( args )... ) ) >::type ResourceType;
  return std::unique_ptr< ResourceType, void(*)( ResourceType* ) >(
    c( std::forward< Args >( args )... ), d );
}

// Initialize SDL resources
std::unique_ptr< SDL_Window, void(*)( SDL_Window* ) > window(
    make_resource( SDL_CreateWindow, SDL_DestroyWindow,
                   "Game of Life", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
                   SDL_WINDOW_SHOWN ) );

std::unique_ptr< SDL_Renderer, void(*)( SDL_Renderer* ) > renderer(
    make_resource( SDL_CreateRenderer, SDL_DestroyRenderer,
                   window.get(), -1,
                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ) );

SDL_Rect makeRectFromMove( size_t x, size_t y ) {
  SDL_Rect rect;
  
  rect.x = SCREEN_WIDTH  / GRID_WIDTH  * x + RECT_BORDER;
  rect.y = SCREEN_HEIGHT / GRID_HEIGHT * y + RECT_BORDER;
  rect.w = SCREEN_WIDTH  / GRID_WIDTH  - 2 * RECT_BORDER;
  rect.h = SCREEN_HEIGHT / GRID_HEIGHT - 2 * RECT_BORDER;

  return rect;
}

/**
 * \brief Renders the grid
 */
void render( const Grid< int >& grid ) {
  SDL_RenderClear( renderer.get() );
  SDL_SetRenderDrawColor( renderer.get(), 255, 255, 255, 255 );
  SDL_RenderFillRect( renderer.get(), nullptr );

  SDL_Rect rect;

#ifdef DEBUG
  for( size_t i = 0; i < GRID_HEIGHT; ++i ) {
    for( size_t j = 0; j < GRID_WIDTH; ++j ) {
      printf( "%d ", grid[i][j] );
    }
    puts( "" );
  }
#endif  // DEBUG

  for( size_t i = 0; i < GRID_HEIGHT; ++i )
    for( size_t j = 0; j < GRID_WIDTH; ++j ) {
      switch( grid[i][j] ) {
      case 0:
        SDL_SetRenderDrawColor( renderer.get(), 100, 100, 100,   0 );
        break;
      case 1:
        SDL_SetRenderDrawColor( renderer.get(),   0,   0, 255,   0 );
        break;
      case 2:
        SDL_SetRenderDrawColor( renderer.get(), 255,   0,   0,   0 );
        break;
      default:
        // This should never happen, so if we see black cells,
        // we know something is up.
        SDL_SetRenderDrawColor( renderer.get(),   0,   0,   0,   0 );
        break;
      }

      rect = makeRectFromMove( i, j );
      SDL_RenderFillRect( renderer.get(), &rect );
    }

  SDL_RenderPresent( renderer.get() );
}

Move builtin_ai_move( const Grid< int >& ) {
  return random_move();
}

#ifdef DEBUG
void stack_dump( lua_State* L ) {
  int top = lua_gettop( L );
  for( int i = 1; i <= top; ++i ) {
    int t = lua_type( L, i );

    switch( t ) {
    case LUA_TSTRING :
      printf( "'%s'\n", lua_tostring( L, i ) );
      break;

    case LUA_TBOOLEAN :
      puts( lua_toboolean( L, i ) ? "true" : "false" );
      break;

    case LUA_TNUMBER :
      printf( "%g\n", lua_tonumber( L, i ) );
      break;

    default :
      printf( "%s\n", lua_typename( L, t ) );
      break;
    }
  }
  puts( "" );
}
#endif  // DEBUG

Move get_ai_move( const Grid< int >& grid,
                  const std::string& ai_script ) {
  lua_State* L;
  size_t x, y;
  L = luaL_newstate();
  luaL_openlibs( L );

  if( luaL_loadfile( L, ai_script.c_str() ) || lua_pcall( L, 0, 0, 0 ) ) {
    fprintf( stderr, "Cannot run configuration file: %s\n",
             lua_tostring( L, -1 ) );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_ai_move( grid );
  }

  lua_getglobal( L, "move" );

  if( !lua_isfunction( L, -1 ) ) {
    fprintf( stderr, "Script %s does not have a function 'move'",
             ai_script.c_str() );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_ai_move( grid );
  }

  lua_newtable( L );

  for( size_t i = 0; i < GRID_HEIGHT; ++i ) {
    lua_newtable( L );
    for( size_t j = 0; j < GRID_WIDTH; ++j ) {
      lua_pushnumber( L, grid[i][j] );
      lua_rawseti( L, -2, j + 1 );
    }
    lua_rawseti( L, -2, i + 1 );
  }

  int ret = lua_pcall( L, 1, 1, 0 );

  // Handles errors
  switch( ret ) {
  case LUA_ERRRUN :
  case LUA_ERRMEM :
  case LUA_ERRERR :
    fprintf( stderr, "Lua error: %s\n", lua_tostring( L, -1 ) );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_ai_move( grid );
    break;

  default :
    break;
  }

  lua_pushstring( L, "x" );
  lua_gettable( L, -2 );

  x = static_cast< size_t >( lua_tointegerx( L, -1, &ret ) );

  if( !ret ) {
    fprintf( stderr, "Got bad value from %s", ai_script.c_str() );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_ai_move( grid );
  }

  lua_pop( L, 1 );

  lua_pushstring( L, "y" );
  lua_gettable( L, -2 );

  y = static_cast< size_t >( lua_tointegerx( L, -1, &ret ) );

  if( !ret ) {
    fprintf( stderr, "Got bad value from %s", ai_script.c_str() );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_ai_move( grid );
  }

  return { x, y };
}

void builtin_mutate_grid( Grid< int >& grid ) {
  Grid< int > new_grid( grid.nRows(), grid.nCols() );

  int neighbors[3] = { 0, 0, 0 };

  for( size_t x = 0; x < GRID_HEIGHT; ++x ) {
    for( size_t y = 0; y < GRID_WIDTH; ++y ) {
      std::fill( std::begin( neighbors ), std::end( neighbors ), 0 );
      for( int i : { -1, 0, 1 } ) {
        for( int j : { -1, 0, 1 } ) {
          if( i == 0 and j == 0 )
            continue;
          else if( !( x == 0 and i == -1 ) and x + i < GRID_HEIGHT and
                   !( y == 0 and j == -1 ) and y + j < GRID_WIDTH )
            ++neighbors[ grid[ x + i ][ y + j ] ];
          else
            ++neighbors[0];
        }
      }

      if( neighbors[0] == 5 or ( grid[x][y] != 0 and neighbors[0] == 6 ) ) {
        auto it = std::max_element( std::next( std::begin( neighbors ) ),
                                    std::end( neighbors ) );
        if( *it == 1 )
          new_grid[x][y] = grid[x][y];
        else
          new_grid[x][y] = it - std::begin( neighbors );
      } else new_grid[x][y] = 0;
    }
  }

  grid = new_grid;
}

void mutate_grid( Grid< int >& grid,
                  const std::string& world_script ) {
  lua_State* L;
  L = luaL_newstate();
  luaL_openlibs( L );

  if( luaL_loadfile( L, world_script.c_str() ) || lua_pcall( L, 0, 0, 0 ) ) {
    fprintf( stderr, "Cannot run configuration file: %s\n",
             lua_tostring( L, -1 ) );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_mutate_grid( grid );
  }

  lua_getglobal( L, "mutate" );

  if( !lua_isfunction( L, -1 ) ) {
    fprintf( stderr, "Script %s does not have a function 'mutate'",
             world_script.c_str() );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_mutate_grid( grid );
  }

  lua_newtable( L );

  for( size_t i = 0; i < GRID_HEIGHT; ++i ) {
    lua_newtable( L );
    for( size_t j = 0; j < GRID_WIDTH; ++j ) {
      lua_pushnumber( L, grid[i][j] );
      lua_rawseti( L, -2, j + 1 );
    }
    lua_rawseti( L, -2, i + 1 );
  }

  int ret = lua_pcall( L, 1, 1, 0 );

  // Handles errors
  switch( ret ) {
  case LUA_ERRRUN :
  case LUA_ERRMEM :
  case LUA_ERRERR :
    fprintf( stderr, "Lua error: %s\n", lua_tostring( L, -1 ) );
    fprintf( stderr, "Defaulting to built-in behavior\n" );
    return builtin_mutate_grid( grid );
    break;

  default :
    break;
  }

  for( size_t i = 0; i < GRID_HEIGHT; ++i ) {
    lua_pushnumber( L, i + 1 );
    lua_gettable( L, -2 );
    for( size_t j = 0; j < GRID_WIDTH; ++j ) {
      lua_pushnumber( L, j + 1 );
      lua_gettable( L, -2 );
      grid[i][j] = lua_tointegerx( L, -1, &ret );
      if( ret == 0 ) {
        fprintf( stderr, "Bad grid returned from script %s\n",
                 world_script.c_str() );
        fprintf( stderr, "Defaulting to built-in behavior\n" );
        return builtin_mutate_grid( grid );
      }
      lua_pop( L, 1 );
    }
    lua_pop( L, 1 );
  }
}

int main( int argc, char* argv[] ) {
  // Parse arguments
  std::string world_script = DEFAULT_WORLD_SCRIPT;
  std::string ai_script = DEFAULT_AI_SCRIPT;
  
  for( int i = 1; i < argc; ++i ) {
    if( strcmp( argv[i], "-w"      ) == 0 or
        strcmp( argv[i], "--world" ) == 0 ) {
      if( ++i < argc ) {
        world_script = argv[i];
        continue;
      }
    } else if( strcmp( argv[i], "-a"   ) == 0 or
               strcmp( argv[i], "--ai" ) == 0 ) {
      if( ++i < argc ) {
        ai_script = argv[i];
        continue;
      }
    }

    fprintf( stderr, HELP_STR, argv[0] );
    return 0;
  }


game_start :
  // Initialize grid
  Grid< int > grid( GRID_WIDTH, GRID_HEIGHT );
  std::vector< int > vec( GRID_SIZE / 4, 0 );
  std::iota( std::begin( vec ), std::end( vec ), 0 );
  std::random_shuffle( std::begin( vec ), std::end( vec ) );
  vec.resize( N_STARTING_CELLS );

  size_t x, y;

  for( int i : vec ) {
    x = 2 * i / GRID_WIDTH;
    y = GRID_HEIGHT / 2 + i % ( GRID_HEIGHT / 2 );
    grid[x][y] = 1;
    grid[ GRID_WIDTH - 1 - x ][ GRID_HEIGHT - 1 - y ] = 2;
  }

  // Main game loop
  while( true ) {
    render( grid );

    // Check to see if game over; if so, let player close window or restart
    size_t p_one_count = 0;
    size_t p_two_count = 0;

    for( size_t i = 0; i < GRID_WIDTH; ++i )
      for( size_t j = 0; j < GRID_HEIGHT; ++j )
        if( grid[i][j] == 1 )
          ++p_one_count;
        else if( grid[i][j] == 2 )
          ++p_two_count;

    if( p_one_count == 0 or p_two_count == 0 ) {
      // Game over
      SDL_Event event;
      while( true ) {
        while( SDL_PollEvent( &event ) ) {
          switch( event.type ) {
          case SDL_QUIT :
            SDL_Quit();
            return 0;
            break;

          case SDL_MOUSEBUTTONDOWN :
            goto game_start;
            break;

          default:
            break;
          }
        }
      }
    }

    // Get move from player
    Move p_one_move;

    bool got_move = false;
    SDL_Event event;
    while( !got_move ) {
      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
        case SDL_QUIT :
          SDL_Quit();
          return 0;
          break;
  
        case SDL_MOUSEBUTTONDOWN :
          got_move = true;
          p_one_move.x = event.button.x * GRID_WIDTH  / SCREEN_WIDTH;
          p_one_move.y = event.button.y * GRID_HEIGHT / SCREEN_HEIGHT;
          break;

        default:
          break;
        }
      }
    }


    // Get move from AI script
    Move p_two_move = get_ai_move( grid, ai_script );


    // Update grid with moves
    if( p_one_move != p_two_move ) {
      grid[ p_one_move.x ][ p_one_move.y ] = 1;
      grid[ p_two_move.x ][ p_two_move.y ] = 2;
    }

    // Update screen with moves
    SDL_Rect rect;
    if( p_one_move == p_two_move ) {
      SDL_SetRenderDrawColor( renderer.get(),  50,  50,  50,   0 );
      rect = makeRectFromMove( p_one_move.x, p_one_move.y );
      SDL_RenderFillRect( renderer.get(), &rect );
    } else {
      SDL_SetRenderDrawColor( renderer.get(),   0,   0, 128,   0 );
      rect = makeRectFromMove( p_one_move.x, p_one_move.y );
      SDL_RenderFillRect( renderer.get(), &rect );

      SDL_SetRenderDrawColor( renderer.get(), 128,   0,   0,   0 );
      rect = makeRectFromMove( p_two_move.x, p_two_move.y );
      SDL_RenderFillRect( renderer.get(), &rect );
    }

    SDL_RenderPresent( renderer.get() );


    // Wait for a moment so the player can see the moves
    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t2;
    std::chrono::duration< double > time_span;
    while( true ) {
      t2 = std::chrono::steady_clock::now();
      time_span =
        std::chrono::duration_cast< std::chrono::duration< double > >(
          t2 - t );
      if( time_span.count() > 1.0 )
        break;
    }


    // Get new grid from World script
    mutate_grid( grid, world_script );
  }

  return 0;
}
