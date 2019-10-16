#ifndef LINE_H
#define LINE_H

#include "game_item.h"

class Line : public GameItem {
public:
  using factory = Factory<Line>;

  Line(float x1, float y1, float x2, float y2)
      : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    shader_id_ = "simple";
  }

  Type GetType() const override { return LINE; }

  float x1_, x2_, y1_, y2_;
};

#endif // LINE_H
