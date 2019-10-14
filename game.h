#ifndef GAME_H
#define GAME_H

#include "scene.h"

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

  void loadScenes();

  GLFWwindow *window_;
  int height_;
  int width_;
  std::vector<Scene *> scenes_;
  Scene *current_scene_;
};

#endif // GAME_H
