#include "./Client.h"

#include <util/port.h>

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// This is stupid, but if we don't do this, then our code will think that a call
// to connect() is supposed to be Client::connect, and it won't compile.
int (&cnct)(int, const sockaddr*, socklen_t) = connect;

void Client::connect() {
  struct addrinfo hints, * servinfo, * p;
  int rv;

  memset( &hints, 0, sizeof hints );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if( ( rv = getaddrinfo( "localhost", PORT, &hints, &servinfo ) ) != 0 ) {
    fprintf( stderr, "getaddrinfo, %s\n", gai_strerror( rv ) );
    return;
  }

  for( p = servinfo; p != nullptr; p = p->ai_next ) {
    if( ( sockfd = socket( p->ai_family,
                           p->ai_socktype,
                           p->ai_protocol ) ) == -1 ) {
      perror( "client: socket" );
      continue;
    } else if( cnct( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) { // See above
      close( sockfd );
      perror( "client: connect" );
      continue;
    } else break;
  }

  if( p == nullptr ) {
    fputs( "client: failed to connect", stderr );
    return;
  }

  freeaddrinfo( servinfo );
}

template < size_t N >
void Client::send( char (&data)[N] ) {
  if( send( sockfd, data, N, 0 ) == -1 )
    perror( "send" );
}

template < size_t N >
void Client::receive( char (&data)[N] ) {
  int numbytes;
  if( ( numbytes = recv( sockfd, data, N, 0 ) ) == -1 )
    perror( "recv" );
}