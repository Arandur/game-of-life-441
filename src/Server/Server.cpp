#include "./Server.h"

#include <util/port.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <list>

#define BACKLOG 10

// get sockaddr, IPv4 or IPv6
void* get_in_addr( struct sockaddr* sa ) {
  if( sa->sa_family == AF_INET )
    return &( ( ( struct sockaddr_in* )( sa ) )->sin_addr );

  return &( ( ( struct sockaddr_in6* )( sa ) )->sin6_addr );
}

Server::Server() {
  struct addrinfo hints, * servinfo, * p;
  int yes = 1;
  int rv;

  memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // use my IP

  rv = getaddrinfo( nullptr, PORT, &hints, &servinfo );
  if( rv != 0 ) {
    throw std::runtime_error( gai_strerror( rv ) );
  }

  // loop through all the results and bind to the first we can
  for( p = servinfo; p != nullptr; p = p->ai_next ) {
    if( ( sockfd = socket( p->ai_family, p->ai_socktype,
                           p->ai_protocol ) ) == -1 ) {
      continue;
    } else if( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                           sizeof( int ) ) == -1 ) {
      perror( "setsockopt" );
      throw std::runtime_error( "" );
    } else if( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
      close( sockfd );
      continue;
    } else break;
  }

  if( p == nullptr ) {
    throw std::runtime_error( "server: failed to bind" );
  }

  freeaddrinfo( servinfo );  // Don't need this structure anymore!

  if( listen( sockfd, BACKLOG ) == -1 ) {
    perror( "listen" );
    throw std::runtime_error( "" );
  }
}

void Server::run() {
  Game* new_game;
  std::string new_game_addr;
  int new_fd, p_one_fd, p_two_fd;
  struct sockaddr_storage their_addr;  // connector's address information
  socklen_t sin_size = sizeof their_addr;
  bool have_first_player = false;
  std::list< std::thread > games;
  char s[ INET6_ADDRSTRLEN ];

  while( true ) {
    new_fd = accept( sockfd, ( struct sockaddr* )( &their_addr ), &sin_size );
    if( new_fd == -1 ) {
      perror( "accept" );
      continue;
    }

    if( ! have_first_player ) {
      p_one_fd = new_fd;
      inet_ntop( their_addr.ss_family,
                 get_in_addr( ( struct sockaddr* )( &their_addr ) ),
                 s, sizeof s );
      have_first_player = true;
    } else {
      p_two_fd = new_fd;
      inet_ntop( their_addr.ss_family,
                 get_in_addr( ( struct sockaddr* )( &their_addr ) ),
                 s, sizeof s );
      have_first_player = false;

      new_game = new Game( p_one_fd, p_two_fd );

      games.emplace_back( &Game::run, new_game );
      games.back().detach();
    }
  }
}