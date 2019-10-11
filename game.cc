#include "game.h"
#include "circle.h"
#include "line.h"
#include "physics.h"
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <algorithm>
#include <cmath>
#include <iostream>

Game::Game() : height_(480), width_(640) {}

Game::~Game() { glfwTerminate(); }

bool CollisionEntryFunc(GameItem *a, GameItem *b) {
  if (a->GetType() == GameItem::CIRCLE && b->GetType() == GameItem::CIRCLE) {
    return Collision((Circle *)a, (Circle *)b);
  } else if (a->GetType() == GameItem::CIRCLE &&
             b->GetType() == GameItem::LINE) {
    return Collision((Circle *)a, (Line *)b);
  }
}

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

  GameItem *item = Circle::factory::GetNewInstance(0.1f, 1000);
  item->r_ = 0.2f;
  item->g_ = 0.7f;
  item->b_ = 0.4f;
  items_.push_back(item);
  for (int i = 1; i < 6; i++) {
    Circle *new_item = Circle::factory::GetNewInstance(0.1f, 1);
    new_item->x_ = -0.9 + 0.3 * i;
    new_item->y_ = 0.3f;
    items_.push_back(new_item);
  }

  Line *line;
  line = Line::factory::GetNewInstance(-0.9f, 0.9f, 0.9f, 0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(-0.9f, -0.9f, 0.9f, -0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(-0.9f, 0.9f, -0.9f, -0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(0.9f, 0.9f, 0.9f, -0.9f);
  items_.push_back(line);
  gate_ = Line::factory::GetNewInstance(-0.2, 0.9f, 0.2f, 0.9f);
  gate_->b_ = gate_->g_ = 0;
  items_.push_back(gate_);

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
  absl::flat_hash_set<GameItem *> to_be_removed;
  for (int i = 0; i < items_.size(); i++) {
    for (int j = i + 1; j < items_.size(); j++) {
      if (CollisionEntryFunc(items_[i], items_[j])) {
        if (items_[j] == gate_) {
          to_be_removed.insert(items_[i]);
        }
      }
    }
  }
  items_.erase(std::remove_if(items_.begin(), items_.end(),
                              [&](GameItem *item) {
                                return to_be_removed.contains(item);
                              }),
               items_.end());
  to_be_removed.clear();
  for (int i = 1; i < items_.size(); i++) {
    if (items_[i]->vx_ == 0 && items_[i]->vy_ == 0) {
      items_[i]->ax_ = items_[i]->ay_ = 0;
    } else {
      float ai = 0.00005f;
      float norm = std::sqrt(items_[i]->vx_ * items_[i]->vx_ +
                             items_[i]->vy_ * items_[i]->vy_);
      float cos = -items_[i]->vx_ / norm;
      float sin = -items_[i]->vy_ / norm;
      items_[i]->ax_ = ai * cos;
      items_[i]->ay_ = ai * sin;
    }
  }
  items_[0]->x_ += items_[0]->vx_;
  items_[0]->y_ += items_[0]->vy_;
  items_[0]->vx_ = items_[0]->vy_ = 0;

  for (int i = 1; i < items_.size(); i++) {
    items_[i]->x_ += items_[i]->vx_;
    items_[i]->y_ += items_[i]->vy_;
    for (int j = i + 1; j < items_.size(); j++) {
      float dx = items_[i]->x_ - items_[j]->x_;
      float dy = items_[i]->y_ - items_[j]->y_;
      float distance = std::sqrt(dx * dx + dy * dy);
    }
    // Update Friction effect.
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
