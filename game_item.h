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
  GameItem() : x_(0), y_(0), vx_(0), vy_(0), ax_(0), ay_(0), m_(1) {}
  virtual void Draw() const = 0;
  virtual Type GetType() const = 0;
  float x_, y_;
  float vx_, vy_;
  float ax_, ay_;
  float m_;
};

#endif // GAME_ITEM_H_
