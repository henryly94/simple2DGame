#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "game_item.h"
#include <cmath>

class Circle : public GameItem {
public:
  using factory = Factory<Circle>;

  Circle(float radius, float mass) : radius_(radius) {
    m_ = mass > 0 ? mass : 1;
    shader_id_ = "simple";
  }

  Type GetType() const override { return CIRCLE; }

  float radius_;
};

#endif // CIRCLE_H_
