#ifndef GAME_H
#define GAME_H

#define GLFW_INCLUDE_NONE
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

  GLFWwindow *window_;
  int height_;
  int width_;
  std::vector<GameItem *> items_;
  Line *gate_;
};

#endif // GAME_H
