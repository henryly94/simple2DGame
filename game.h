#ifndef GAME_H
#define GAME_H

#define GLFW_INCLUDE_NONE
#include "circle.h"
#include "game_item.h"
#include "line.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
};

#endif // GAME_H
