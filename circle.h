#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "game_item.h"
#include <cmath>

class Circle : public GameItem {
public:
  using factory = Factory<Circle>;

  Circle(float radius, float mass) : radius_(radius) {
    m_ = mass > 0 ? mass : 1;
    float vertices[200];
    for (int i = 0; i < 100; i++) {
      float theta = 2.0f * 3.1415926f * float(i) / 100.0f;
      float x = radius_ * std::cos(theta);
      float y = radius_ * std::sin(theta);
      vertices[i * 2] = x;
      vertices[i * 2 + 1] = y;
    }
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Draw() const override {
    glUseProgram(shader_program());
    glUniform3f(glGetUniformLocation(shader_program(), "color"), r_, g_, b_);
    glUniform2f(glGetUniformLocation(shader_program(), "pos"), x_, y_);
    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 200);
    glBindVertexArray(0);
  }

  Type GetType() const override { return CIRCLE; }

  float radius_;
  unsigned int VAO_, VBO_;
};

#endif // CIRCLE_H_
