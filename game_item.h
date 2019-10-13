#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#define GLFW_INCLUDE_NONE
#include "factory.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
class GameItem {
public:
  using factory = Factory<GameItem>;
  enum Type {
    CIRCLE,
    LINE,
  };

  enum Group { SCENE, BACKGROUND, PLAYER };

  GameItem()
      : x_(0), y_(0), vx_(0), vy_(0), ax_(0), ay_(0), m_(1), r_(1), g_(1),
        b_(1), visible_(true), exist_(true), group_(BACKGROUND) {}
  virtual void Draw() const = 0;
  virtual Type GetType() const = 0;
  float x_, y_;
  float vx_, vy_;
  float ax_, ay_;
  float m_;
  float r_, g_, b_;
  Group group_;
  bool visible_;
  bool exist_;
};

#endif // GAME_ITEM_H_
