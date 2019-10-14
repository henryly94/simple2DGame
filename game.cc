#include "game.h"
#include "game_scene.h"
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <algorithm>
#include <cmath>
#include <iostream>

Game::Game() : height_(640), width_(640) {}

Game::~Game() {
  glfwTerminate();
  for (auto *scene : scenes_) {
    delete scene;
  }
}

bool Game::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  loadScenes();
  return true;
}

void Game::MainLoop() {
  while (!glfwWindowShouldClose(window_)) {

    processInput();

    update();

    render();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Game::processInput() {
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_, true);
  }
  current_scene_->ProcessInput();
}

void Game::update() {
  static absl::Time time = absl::Now();
  static absl::Duration interval = absl::Milliseconds(10);
  absl::Time new_time = absl::Now();
  if (new_time - time < interval) {
    return;
  }
  current_scene_->Update();
  time = new_time;
}

void Game::render() { current_scene_->Draw(); }

void Game::loadScenes() {
  GameScene *gameScene = new GameScene(window_);
  scenes_.push_back(gameScene);
  current_scene_ = gameScene;
}
