#include "game.h"

#include <string>

int main(int argc, char **argv) {
  std::string id = argv[1];
  std::string ip = argv[2];
  std::string port = argv[3];
  Game game(id, ip, port);
  game.Init();
  game.MainLoop();
  return 0;
}
