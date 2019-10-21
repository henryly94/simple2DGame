#include "game.h"

#include <string>

int main(int argc, char **argv) {
  std::string id = argv[1];
  Game game(id);
  game.Init();
  game.MainLoop();
  return 0;
}
