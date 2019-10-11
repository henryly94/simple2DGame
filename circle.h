#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "game_item.h"
#include <cmath>

class Circle : public GameItem {
public:
  using factory = Factory<Circle>;

  Circle(float radius, float mass) : radius_(radius) {
    m_ = mass > 0 ? mass : 1;
  }

  void Draw() const override {
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 100; i++) {
      float theta = 2.0f * 3.1415926f * float(i) / 100.0f;
      float x = radius_ * std::cos(theta);
      float y = radius_ * std::sin(theta);
      glVertex2f(x + x_, y + y_);
    }
    glEnd();
  }

  Type GetType() const override { return CIRCLE; }

  float radius_;
};

#endif // CIRCLE_H_
