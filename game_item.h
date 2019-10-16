#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#include "factory.h"
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
      : x_(0), y_(0), vx_(0), vy_(0), ax_(0), ay_(0), m_(1), r_(1), g_(1),
        b_(1), visible_(true), exist_(true), group_(BACKGROUND) {}
  virtual Type GetType() const = 0;

  float x_, y_;
  float vx_, vy_;
  float ax_, ay_;
  float m_;
  float r_, g_, b_;
  Group group_;
  bool visible_;
  bool exist_;
  std::string shader_id_;
  std::string texture_id_;
};

#endif // GAME_ITEM_H_
