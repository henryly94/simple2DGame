#include "game.h"
#include "circle.h"
#include "letter.h"
#include "line.h"
#include "physics.h"
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

void updateBall(GameItem *item) {
  if (item->vx_ == 0 && item->vy_ == 0) {
    item->ax_ = item->ay_ = 0;
  } else {
    float ai = 0.00008f;
    float norm = std::sqrt(item->vx_ * item->vx_ + item->vy_ * item->vy_);
    float cos = -item->vx_ / norm;
    float sin = -item->vy_ / norm;
    item->ax_ = ai * cos;
    item->ay_ = ai * sin;
  }
  item->x_ += item->vx_;
  item->y_ += item->vy_;
  // Update Friction effect.
  if (item->vx_ != 0) {
    float new_vx = item->vx_ + item->ax_;
    if (new_vx * item->vx_ < 0) {
      item->vx_ = 0;
    } else {
      item->vx_ = new_vx;
    }
  }
  if (item->vy_ != 0) {
    float new_vy = item->vy_ + item->ay_;
    if (new_vy * item->vy_ < 0) {
      item->vy_ = 0;
    } else {
      item->vy_ = new_vy;
    }
  }
}

} // namespace

Game::Game() : height_(640), width_(640) {}

Game::~Game() { glfwTerminate(); }

bool CollisionEntryFunc(GameItem *a, GameItem *b) {
  if (a->GetType() == GameItem::CIRCLE && b->GetType() == GameItem::CIRCLE) {
    return Collision((Circle *)a, (Circle *)b);
  } else if (a->GetType() == GameItem::CIRCLE &&
             b->GetType() == GameItem::LINE) {
    return Collision((Circle *)a, (Line *)b);
  }
  return false;
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
  loadItems();
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
    player1_->vy_ = 0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    player1_->vy_ = -0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    player1_->vx_ = -0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    player1_->vx_ = 0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
    player2_->vy_ = 0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
    player2_->vy_ = -0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
    player2_->vx_ = -0.01f;
  } else if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    player2_->vx_ = 0.01f;
  }
  if (glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS) {
    restoreItems();
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
    if (items_[i]->exist_) {
      for (int j = i + 1; j < items_.size(); j++) {
        if (items_[j]->exist_ && CollisionEntryFunc(items_[i], items_[j])) {
          if (items_[i] == ball_) {
            if (items_[j] == gate1_) {
              to_be_removed.insert(items_[i]);
              point2_->ch_ += 1;
              point2_->UpdateChar();
            } else if (items_[j] == gate2_) {
              to_be_removed.insert(items_[i]);
              point1_->ch_ += 1;
              point1_->UpdateChar();
            }
          }
        }
      }
    }
  }
  for (auto *item : to_be_removed) {
    item->exist_ = false;
    item->visible_ = false;
  }
  to_be_removed.clear();
  for (auto *item : items_) {
    if (item->exist_) {
      if (item == player1_ || item == player2_) {
        item->x_ += item->vx_;
        item->y_ += item->vy_;
        item->vx_ = item->vy_ = 0;
      } else if (item == ball_) {
        updateBall(item);
      }
    }
  }

  time = new_time;
}

void Game::render() {
  glClearColor(0.25f, 0.75f, 0.45f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto *item : items_) {
    if (item->visible_) {
      item->Draw();
    }
  }
}

void Game::loadItems() {
  player1_ = Circle::factory::GetNewInstance(0.1f, 1000);
  player1_->r_ = 0.93f;
  player1_->g_ = 0.39f;
  player1_->b_ = 0.39f;
  player1_->y_ = 0.5f;
  player1_->x_ = 0.1;
  player1_->group_ = GameItem::PLAYER;
  items_.push_back(player1_);

  player2_ = Circle::factory::GetNewInstance(0.1f, 1000);
  player2_->r_ = 0.55f;
  player2_->g_ = 0.95f;
  player2_->b_ = 1.0f;
  player2_->group_ = GameItem::PLAYER;
  player2_->y_ = -0.5f;
  player2_->x_ = 0.1f;
  items_.push_back(player2_);

  ball_ = Circle::factory::GetNewInstance(0.1f, 1);
  ball_->y_ = 0.0f;
  ball_->x_ = 0.1f;
  ball_->group_ = GameItem::SCENE;
  items_.push_back(ball_);

  Line *line;
  line = Line::factory::GetNewInstance(-0.7f, 0.9f, 0.9f, 0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(-0.7f, -0.9f, 0.9f, -0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(-0.7f, 0.9f, -0.7f, -0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(0.9f, 0.9f, 0.9f, -0.9f);
  items_.push_back(line);
  line = Line::factory::GetNewInstance(-0.7f, 0.0f, 0.9f, 0.0f);
  line->exist_ = false;
  items_.push_back(line);
  gate1_ = Line::factory::GetNewInstance(-0.1f, 0.9f, 0.3f, 0.9f);
  gate1_->b_ = gate1_->g_ = gate1_->r_ = 0;
  items_.push_back(gate1_);
  gate2_ = Line::factory::GetNewInstance(-0.1f, -0.9f, 0.3f, -0.9f);
  gate2_->b_ = gate2_->g_ = gate2_->r_ = 0;
  items_.push_back(gate2_);

  point1_ = Letter::factory::GetNewInstance('0', -0.85f, 0.8f, 0.15f, 0.2f);
  point1_->r_ = point1_->g_ = point1_->b_ = 1.0f;
  point2_ = Letter::factory::GetNewInstance('0', -0.85f, -0.8f, 0.15f, 0.2f);
  point2_->r_ = point2_->g_ = point2_->b_ = 1.0f;
  items_.push_back(point1_);
  items_.push_back(point2_);
}

void Game::restoreItems() {
  ball_->x_ = 0.0f;
  ball_->y_ = 0.0f;
  ball_->exist_ = true;
  ball_->visible_ = true;
  player1_->x_ = 0.0f;
  player1_->y_ = 0.5f;
  player2_->x_ = 0.0f;
  player2_->y_ = -0.5f;
}
