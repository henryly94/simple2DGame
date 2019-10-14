#ifndef LINE_H
#define LINE_H

#include "game_item.h"

class Line : public GameItem {
public:
  using factory = Factory<Line>;

  Line(float x1, float y1, float x2, float y2)
      : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    float vertices[] = {x1_, y1_, x2_, y2_};
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
    glUniform2f(glGetUniformLocation(shader_program(), "pos"), 0, 0);
    glBindVertexArray(VAO_);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
  }

  Type GetType() const override { return LINE; }

  float x1_, x2_, y1_, y2_;
  unsigned int VAO_, VBO_;
};

#endif // LINE_H
