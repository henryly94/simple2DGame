#ifndef LETTER_H_
#define LETTER_H_

#include <iostream>

#include "game_item.h"

class Letter : public GameItem {
public:
  using factory = Factory<Letter>;
  Letter(char ch, float x, float y, float w, float h) : ch_(ch), w_(w), h_(h) {
    x_ = x;
    y_ = y;
    texture_id_ = std::string{1, ch};
    shader_id_ = "letter";
  }

  Type GetType() const override { return LETTER; }

  char ch_;
  float w_, h_;
};

#endif // LETTER_H_
