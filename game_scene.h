#ifndef GAME_SCENE_H_
#define GAME_SCENE_H_

#include "circle.h"
#include "game_item.h"
#include "letter.h"
#include "line.h"
#include "scene.h"

class GameScene : public Scene {
public:
  GameScene(GLFWwindow *window);

  void ProcessInput() override;

  void Update() override;

  void loadItems();

  void restoreItems();

  Circle *player1_, *player2_, *ball_;
  Line *gate1_, *gate2_;
  Letter *point1_, *point2_;
};

#endif // GAME_SCENE_H_
