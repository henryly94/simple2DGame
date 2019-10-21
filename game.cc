#include "game.h"
#include "game_scene.h"
#include "welcome_scene.h"
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "shader.h"
#include "texture.h"

#include <ft2build.h>

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include FT_FREETYPE_H

Game::Game(std::string &id) : height_(640), width_(640), id_(id) {}

Game::~Game() {
  for (auto *scene : scenes_) {
    delete scene;
  }
  delete renderer_;
  glfwTerminate();
}

bool Game::Init() {
  std::cout << "Here\n";
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  window_ = glfwCreateWindow(width_, height_, "Main", NULL, NULL);
  if (window_ == NULL) {
    glfwTerminate();
    std::cout << "No window\n";
    return false;
  }
  glfwMakeContextCurrent(window_);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "No glad\n";
    return false;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << "Here\n";
  renderer_ = new Renderer(window_);
  std::cout << "Here\n";
  controller_ = new Controller(window_);
  std::cout << "Here\n";
  Shader::Bind("simple", "../shaders/simple.vs", "../shaders/simple.fs");
  Shader::Bind("letter", "../shaders/letter.vs", "../shaders/letter.fs");

  FT_Library library;         // 声明了Lib
  FT_Init_FreeType(&library); // 初始化 返回0为成功:w
  FT_Face face;
  int error = FT_New_Face(library, "../resource/Consolas.ttf", 0, &face);
  FT_Set_Pixel_Sizes(face, 0, 128); // 设立字体为128px
  for (char ch = 127; ch != 0; ch--) {
    FT_Load_Char(face, ch, FT_LOAD_RENDER);
    FT_GlyphSlot &slot = face->glyph; // 输出结果在 slot->bitmap
    auto &bitmap = slot->bitmap;
    Texture::Bind(std::string{1, ch}, bitmap.buffer, bitmap.width, bitmap.rows,
                  GL_RED, GL_RED);
  }
  loadScenes();
  return true;
}

void Game::MainLoop() {
  while (!glfwWindowShouldClose(window_)) {
    controller_->ClearUpdates();

    processInput();
    std::cout << "A\n";
    update();

    std::cout << "B\n";
    render();
  }
}

void Game::processInput() {
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_, true);
  }
  static bool not_pressed_space = true;
  if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
    static int index = 1;
    if (not_pressed_space) {
      current_scene_ = scenes_[index];
      index = 1 - index;
      if (index == 0) {
        renderer_->SwitchScene("game");
      } else {
        renderer_->SwitchScene("welcome");
      }
      not_pressed_space = false;
    }
  } else {
    not_pressed_space = true;
  }
  current_scene_->ProcessInput();
  controller_->ProcessInput(id_);
  // std::cout << controller_->update_protos_.DebugString() << "--------\n";
}

void Game::update() {
  static absl::Time time = absl::Now();
  static absl::Duration interval = absl::Milliseconds(10);
  absl::Time new_time = absl::Now();
  if (new_time - time < interval) {
    return;
  }
  current_scene_->Update(controller_->update_protos_);
  time = new_time;
}

void Game::render() { renderer_->Render(); }

void Game::loadScenes() {
  WelcomeScene *welcomeScene = new WelcomeScene(window_);
  scenes_.push_back(welcomeScene);
  GameScene *gameScene = new GameScene(window_);
  scenes_.push_back(gameScene);
  current_scene_ = welcomeScene;
  renderer_->AddScene("welcome", welcomeScene);
  renderer_->AddScene("game", gameScene);
  renderer_->SwitchScene("welcome");
}
