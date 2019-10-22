#include "game_scene.h"
#include "physics.h"

namespace {

void updateBall(GameItem *item) {
  if (item->vx_ == 0 && item->vy_ == 0) {
    item->ax_ = item->ay_ = 0;
  } else {
    float ai = 0.00006f;
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

GameScene::GameScene() : Scene() {
  bg_color_ = {0.25f, 0.75f, 0.45f};
  loadItems();
}

/*
  if (glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS) {
    RestoreItems();
  }
*/

void GameScene::Update(const UpdateProtos &update_protos) {
  absl::flat_hash_set<GameItem *> to_be_removed;
  for (const auto update : update_protos.updates()) {
    if (update.id() == "player1") {
      player1_->vx_ = update.physics().vx();
      player1_->vy_ = update.physics().vy();
    } else if (update.id() == "player2") {
      player2_->vx_ = update.physics().vx();
      player2_->vy_ = update.physics().vy();
    }
  }
  for (int i = 0; i < items_.size(); i++) {
    if (items_[i]->exist_) {
      for (int j = i + 1; j < items_.size(); j++) {
        if (items_[j]->exist_ && CollisionEntryFunc(items_[i], items_[j])) {
          if (items_[i] == ball_) {
            if (items_[j] == gate1_) {
              to_be_removed.insert(items_[i]);
              point2_->ch_ += 1;
              point2_->texture_id_ = std::string{1, point2_->ch_};
            } else if (items_[j] == gate2_) {
              to_be_removed.insert(items_[i]);
              point1_->ch_ += 1;
              point1_->texture_id_ = std::string{1, point1_->ch_};
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

  updateBall(ball_);

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
}

void GameScene::loadItems() {
  player1_ = Circle::factory::GetNewInstance(0.1f, 1000);
  player1_->color_ = {0.93f, 0.39f, 0.39f};
  player1_->y_ = 0.5f;
  player1_->x_ = 0.1;
  player1_->group_ = GameItem::PLAYER;
  items_.push_back(player1_);

  player2_ = Circle::factory::GetNewInstance(0.1f, 1000);
  player2_->color_ = {0.55f, 0.95f, 1.0f};
  player2_->group_ = GameItem::PLAYER;
  player2_->y_ = -0.5f;
  player2_->x_ = 0.1f;
  items_.push_back(player2_);

  ball_ = Circle::factory::GetNewInstance(0.1f, 1);
  ball_->y_ = 0.0f;
  ball_->x_ = 0.1f;
  ball_->group_ = GameItem::SCENE;
  items_.push_back(ball_);

  gate1_ = Line::factory::GetNewInstance(-0.1f, 0.9f, 0.3f, 0.9f);
  gate1_->color_ = {0.0f, 0.0f, 0.0f};
  items_.push_back(gate1_);
  gate2_ = Line::factory::GetNewInstance(-0.1f, -0.9f, 0.3f, -0.9f);
  gate2_->color_ = {0.0f, 0.0f, 0.0f};
  items_.push_back(gate2_);

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

  point1_ = Letter::factory::GetNewInstance('0', -0.85f, 0.8f, 0.15f, 0.2f);
  point1_->color_ = {1.0f, 1.0f, 1.0f};
  point2_ = Letter::factory::GetNewInstance('0', -0.85f, -0.8f, 0.15f, 0.2f);
  point2_->color_ = {1.0f, 1.0f, 1.0f};
  items_.push_back(point1_);
  items_.push_back(point2_);
}

void GameScene::RestoreItems() {
  ball_->x_ = 0.0f;
  ball_->y_ = 0.0f;
  ball_->exist_ = true;
  ball_->visible_ = true;
  player1_->x_ = 0.0f;
  player1_->y_ = 0.5f;
  player2_->x_ = 0.0f;
  player2_->y_ = -0.5f;
}
