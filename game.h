#ifndef GAME_H
#define GAME_H

#include <string>

#include "controller.h"
#include "renderer.h"
#include "scene.h"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

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

  boost::asio::io_context network_io_;
  boost::thread *network_thread_ptr_;
};

#endif // GAME_H
