#ifndef GAME_SCENE_H_
#define GAME_SCENE_H_

#include "circle.h"
#include "game_item.h"
#include "letter.h"
#include "line.h"
#include "scene.h"

class GameScene : public Scene {
public:
  GameScene();

  void Update(const UpdateProtos &update_protos) override;

  void Update(const UpdateProto &update_proto) override;

  void loadItems();

  void RestoreItems();

  Circle *player1_, *player2_, *ball_;
  Line *gate1_, *gate2_;
  Letter *point1_, *point2_;
};

#endif // GAME_SCENE_H_
