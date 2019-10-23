#ifndef WELCOME_SCENE_H_
#define WELCOME_SCENE_H_

#include "letter.h"
#include "scene.h"

#include <vector>

class WelcomeScene : public Scene {
public:
  WelcomeScene() : Scene() {
    float x = -0.8f, y = 0.0f;
    for (const char &ch : "Welcome") {
      Letter *letter = Letter::factory::GetNewInstance(ch, x, y, 0.2f, 0.15f);
      letter->color_ = {0.9f, 0.7f, 0.2f};
      x += 0.2f;
      letters_.push_back(letter);
      items_.push_back(letter);
    }
    bg_color_ = {1.0f, 1.0f, 1.0f};
  }

  void Update(const UpdateProtos &update_protos) override {}

  void Update(const UpdateProto &update_proto) override {}

  std::vector<Letter *> letters_;
};

#endif // WELCOME_SCENE_H_
