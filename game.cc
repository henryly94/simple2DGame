#include "game.h"
#include "circle.h"
#include "physics.h"
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <iostream>
#include <algorithm>
#include <cmath>

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

  GameItem *item = Circle::factory::GetNewInstance(0.5f, 1000);
  items_.push_back(item);
  GameItem *item2 = Circle::factory::GetNewInstance(0.1f, 1);
  item2->vx_ = 0.02f;
  item2->vy_ = 0.02f;
  item2->x_ = -0.6f;
  item2->y_ = -0.6f;
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
    items_[0]->vy_ = 0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    items_[0]->vy_ = -0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    items_[0]->vx_ = -0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    items_[0]->vx_ = 0.01f;
  }
}

void Game::update() {
  static absl::Time time = absl::Now();
  static absl::Duration interval = absl::Milliseconds(10);
  absl::Time new_time = absl::Now();
  if (new_time - time < interval) {
    return;
  }
  for (int i=1; i<items_.size(); i++){
    for (int j=0; j<items_.size(); j++) {
      if (i == j) continue;
      float dx = items_[i]->x_ - items_[j]->x_;
      float dy = items_[i]->y_ - items_[j]->y_;
      float distance = std::sqrt(dx*dx + dy*dy);
      if (distance <= ((Circle*)items_[i])->radius_ + ((Circle*)items_[j])->radius_) {
        Collision((Circle*)items_[i], (Circle*)items_[j]);
      } else {
        if (items_[i]->x_ <= ((Circle *)items_[i])->radius_ - 1 ||
            items_[i]->x_ >= 1 - ((Circle *)items_[i])->radius_) {
          items_[i]->vx_ = -items_[i]->vx_;
        }if (items_[i]->y_ <= ((Circle *)items_[i])->radius_ - 1 ||
            items_[i]->y_ >= 1 - ((Circle *)items_[i])->radius_) {
          items_[i]->vy_ = -items_[i]->vy_;
        }
      }
    }
  }

  if (items_[1]->vx_ == 0 && items_[1]->vy_ == 0) {
    items_[1]->ax_ = items_[1]->ay_ = 0;
  } else {
    float a1 = 0.00005f;
    float norm = std::sqrt(items_[1]->vx_ * items_[1]->vx_ +
                           items_[1]->vy_ * items_[1]->vy_);
    float cos = -items_[1]->vx_ / norm;
    float sin = -items_[1]->vy_ / norm;
    items_[1]->ax_ = a1 * cos;
    items_[1]->ay_ = a1 * sin;
  }
  items_[0]->x_ += items_[0]->vx_;
  items_[0]->y_ += items_[0]->vy_;
  items_[0]->vx_ = items_[0]->vy_ = 0;

  for (int i=1; i<items_.size(); i++){
    items_[i]->x_ += items_[i]->vx_;
    items_[i]->y_ += items_[i]->vy_;
    if (items_[i]->vx_ != 0) {
      float new_vx = items_[i]->vx_ + items_[i]->ax_;
      if (new_vx * items_[i]->vx_ < 0) {
        items_[i]->vx_ = 0;
      } else {
        items_[i]->vx_ = new_vx;
      }
    }
    if (items_[i]->vy_ != 0) {
      float new_vy = items_[i]->vy_ + items_[i]->ay_;
      if (new_vy * items_[i]->vy_ < 0) {
        items_[i]->vy_ = 0;
      } else {
        items_[i]->vy_ = new_vy;
      }
    }
  }
  time = new_time;
}

void Game::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto *item : items_) {
    item->Draw();
  }
}
