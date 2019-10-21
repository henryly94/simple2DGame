#ifndef GAME_H
#define GAME_H

#include <string>

#include "controller.h"
#include "renderer.h"
#include "scene.h"
class Game {
public:
  Game(std::string &id);
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
  std::string id_;
  std::vector<Scene *> scenes_;
  GLFWwindow *window_;
  Renderer *renderer_;
  Controller *controller_;
  Scene *current_scene_;
};

#endif // GAME_H
