#include "./Client.h"

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
    } else if( ::connect( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) { // See above
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