#pragma once

#include <util/port.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class Client {
public:
  Client() = default;

  void connect();
  template < size_t N >
  void send( char (&data)[N] ) {
    if( ::send( sockfd, data, N, 0 ) == -1 )
      perror( "send" );
  }
  template < size_t N >
  void receive( char (&data)[N] ) {
    int numbytes;
    if( ( numbytes = recv( sockfd, data, N, 0 ) ) == -1 )
      perror( "recv" );
  }

private:
  int sockfd;
};