#include <Server/Server.h>
#include <HumanPlayer/HumanPlayer.h>
#include <ComputerPlayer/ComputerPlayer.h>

#include <thread>
#include <chrono>
#include <cstdio>

int main() {
  std::thread server_thread( &Server::run, new Server );
  //std::thread human_thread( &HumanPlayer::play, new HumanPlayer );
  std::thread computer_thread( &ComputerPlayer::play, new ComputerPlayer );

  HumanPlayer hp;
  hp.play();

  server_thread.detach();
  //human_thread.join();
  computer_thread.join();

  return 0;
}
