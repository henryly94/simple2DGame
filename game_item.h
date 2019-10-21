#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#include "factory.h"
#include "math.h"
#include "shader.h"
#include <iostream>
#include <string>

class GameItem {
public:
  using factory = Factory<GameItem>;
  enum Type {
    CIRCLE,
    LINE,
    LETTER,
  };

  enum Group { SCENE, BACKGROUND, PLAYER };

  GameItem()
      : x_(0), y_(0), vx_(0), vy_(0), ax_(0), ay_(0), m_(1),
        color_(1.0f, 1.0f, 1.0f), visible_(true), exist_(true),
        group_(BACKGROUND) {}
  virtual Type GetType() const = 0;

  float x_, y_;
  float vx_, vy_;
  float ax_, ay_;
  float m_;
  Vec3 color_;
  Group group_;
  bool visible_;
  bool exist_;
  std::string shader_id_;
  std::string texture_id_;
};

#endif // GAME_ITEM_H_
