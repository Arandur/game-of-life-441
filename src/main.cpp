#include <iostream>

#include <Server/Server.h>
#include <HumanPlayer/HumanPlayer.h>
#include <ComputerPlayer/ComputerPlayer.h>
#include <thread>

int main() {
  std::thread server_thread( &Server::run, new Server );
  std::thread human_thread( &HumanPlayer::play, new HumanPlayer );
  std::thread computer_thread( &ComputerPlayer::play, new ComputerPlayer );

  server_thread.detach();
  human_thread.join();
  computer_thread.join();

  return 0;
}
