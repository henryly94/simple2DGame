#ifndef CIRCLE_H_
#define CIRCLE_H_

#define GLFW_INCLUDE_NONE
#include "factory.h"
#include "game_item.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glad/glad.h>

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

  float radius_;
};

#endif // CIRCLE_H_
