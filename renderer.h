#ifndef RENDERER_H_
#define RENDERER_H_

#include "scene.h"

#include <absl/container/flat_hash_map.h>
#include <absl/strings/string_view.h>
#include <string>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

class Renderer {
public:
  Renderer(GLFWwindow *window);

  ~Renderer();

  void AddScene(absl::string_view scene_name, Scene *scene_ptr);

  void SwitchScene(absl::string_view scene_name);

  void Render();

private:
  void renderItem(const GameItem *item);

  unsigned int getVAO(const GameItem *item, bool replace);

  absl::flat_hash_map<const GameItem *, unsigned int> VAO_map_;

  absl::flat_hash_map<std::string, Scene *> scene_map_;
  Scene *current_scene_ = nullptr;
  GLFWwindow *window_ = nullptr;
};

#endif // RENDERER_H_
