#include "game.h"
#include "circle.h"
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

  GameItem* item = Circle::factory::GetNewInstance(0.5f);
  items_.push_back(item);
  GameItem* item2 = Circle::factory::GetNewInstance(0.1f);
  items_.push_back(item2);
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
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    items_[0]->vy_ += 0.01f;
  } 
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    items_[0]->vy_ -= 0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    items_[0]->vx_ -= 0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    items_[0]->vx_ += 0.01f;
  }
}

void Game::update() {
  items_[0]->x_ += items_[0]->vx_;
  items_[0]->y_ += items_[0]->vy_;
  items_[0]->vx_ = items_[0]->vy_ = 0;
  items_[1]->y_ += 0.02f;
  if (items_[1]->y_ > 1.0f) {
    items_[1]->y_ = -1.0f;
  }
  items_[1]->x_ += 0.02f;
  if (items_[1]->x_ > 1.0f) {
    items_[1]->x_ = -1.0f;
  }
}

void Game::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto* item : items_) {
    item->Draw();
  }
}
