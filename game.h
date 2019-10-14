#ifndef GAME_H
#define GAME_H

#include "circle.h"
#include "letter.h"
#include "line.h"
class Game {
public:
  Game();
  ~Game();
  bool Init();
  void MainLoop();

private:
  void processInput();

  void render();

  void update();

  void loadItems();
  void restoreItems();

  GLFWwindow *window_;
  int height_;
  int width_;
  std::vector<GameItem *> items_;
  Circle *player1_, *player2_, *ball_;
  Line *gate1_, *gate2_;
  Letter *point1_, *point2_;
};

#endif // GAME_H
