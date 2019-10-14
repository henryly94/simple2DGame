#ifndef SCENE_H_
#define SCENE_H_

#include "game_item.h"

class Scene {
public:
  Scene(GLFWwindow *window) : window_(window){};

  virtual ~Scene(){};

  virtual void Draw() const = 0;

  virtual void ProcessInput() = 0;

  virtual void Update() = 0;

protected:
  GLFWwindow *window_;
};

#endif // SCENE_H_
