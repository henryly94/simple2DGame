#ifndef GAME_H
#define GAME_H

#include "renderer.h"
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

  int height_;
  int width_;
  std::vector<Scene *> scenes_;
  GLFWwindow *window_;
  Renderer *renderer_;
  Scene *current_scene_;
};

#endif // GAME_H
