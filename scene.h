#ifndef SCENE_H_
#define SCENE_H_

#include "game_item.h"
#include "math.h"
#include "message.pb.h"

#include <boost/thread/mutex.hpp>
#include <vector>

class Scene {
public:
  Scene() : mu_(){};

  virtual ~Scene(){};

  virtual void Update(const UpdateProtos &update_protos) = 0;

  virtual void Update(const UpdateProto &update_proto) = 0;

  std::vector<GameItem *> items_;

  boost::mutex mu_;

  Vec3 bg_color_;

  UpdateProtos protos_;
};

#endif // SCENE_H_
