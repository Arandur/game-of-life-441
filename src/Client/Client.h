#pragma once

#include <cstddef>

class Client {
public:
  Client() = default;

  void connect();
  template < size_t N >
  void send( char (&)[N] );
  template < size_t N >
  void receive( char (&)[N] );

private:
  int sockfd;
};