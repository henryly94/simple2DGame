#ifndef SCENE_H_
#define SCENE_H_

#include "game_item.h"
#include "message.pb.h"

#include <vector>

class Scene {
public:
  Scene(GLFWwindow *window) : window_(window){};

  virtual ~Scene(){};

  virtual void ProcessInput() = 0;

  virtual void Update(const UpdateProtos &update_protos) = 0;

  std::vector<GameItem *> items_;

  float r_, g_, b_;

protected:
  GLFWwindow *window_;
};

#endif // SCENE_H_
