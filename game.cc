#include "game.h"

#include <iostream>

Game::Game() : height_(480), width_(640) {}

Game::~Game() { glfwTerminate(); }

bool Game::Init() {
  glfwInit();
  window_ = glfwCreateWindow(width_, height_, "Main", NULL, NULL);
  if (window_ == NULL) {
    glfwTerminate();
    std::cout << "No window\n";
    return false;
  }
  glfwMakeContextCurrent(window_);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "No glad\n";
    return false;
  }
  return true;
}

void Game::MainLoop() {
  while (!glfwWindowShouldClose(window_)) {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}
