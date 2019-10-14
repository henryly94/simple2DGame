#ifndef WELCOME_SCENE_H_
#define WELCOME_SCENE_H_

#include "letter.h"
#include "scene.h"

#include <vector>

class WelcomeScene : public Scene {
public:
  WelcomeScene(GLFWwindow *window) : Scene(window) {
    float x = -0.8f, y = 0.0f;
    for (const char &ch : "Welcome") {
      Letter *letter = Letter::factory::GetNewInstance(ch, x, y, 0.2f, 0.15f);
      letter->r_ = 0.9f;
      letter->g_ = 0.7f;
      letter->b_ = 0.2f;
      x += 0.2f;
      letters_.push_back(letter);
    }
  }

  void Draw() const override {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto *letter : letters_) {
      letter->Draw();
    }
  }

  void Update() override {}

  void ProcessInput() override {}

  std::vector<Letter *> letters_;
};

#endif // WELCOME_SCENE_H_
