#ifndef GAME_H
#define GAME_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "game_item.h"

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

  GLFWwindow *window_;
  int height_;
  int width_;
  std::vector<GameItem*> items_;
};

#endif // GAME_H
