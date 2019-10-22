#include "renderer.h"
#include "circle.h"
#include "letter.h"
#include "line.h"
#include "texture.h"

#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

namespace {

unsigned int BindCircleVAO(float radius) {
  float vertices[200];
  for (int i = 0; i < 100; i++) {
    float theta = 2.0f * 3.1415926f * float(i) / 100.0f;
    float x = radius * std::cos(theta);
    float y = radius * std::sin(theta);
    vertices[i * 2] = x;
    vertices[i * 2 + 1] = y;
  }
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return VAO;
}

unsigned int BindLineVAO(float x1, float y1, float x2, float y2) {
  float vertices[] = {x1, y1, x2, y2};
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return VAO;
}

unsigned int BindLetterVAO(char ch, float w, float h) {
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  float vertices[] = {-w / 2, -h / 2, 0.0f, 0.0f, w / 2,  -h / 2, 1.0f, 0.0f,
                      w / 2,  h / 2,  1.0f, 1.0f, -w / 2, -h / 2, 0.0f, 0.0f,
                      w / 2,  h / 2,  1.0f, 1.0f, -w / 2, h / 2,  0.0f, 1.0f};
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return VAO;
}

} // namespace

Renderer::Renderer(GLFWwindow *window) : window_(window) {}

Renderer::~Renderer() {}

void Renderer::AddScene(absl::string_view scene_name, Scene *scene_ptr) {
  scene_map_[scene_name] = scene_ptr;
}

void Renderer::SwitchScene(absl::string_view scene_name) {
  auto it = scene_map_.find(scene_name);
  if (it != scene_map_.end()) {
    current_scene_ = it->second;
  }
}

void Renderer::Render() {
  if (current_scene_ != nullptr) {
    boost::mutex::scoped_lock(current_scene_->mu_);
    glClearColor(current_scene_->bg_color_.data[0],
                 current_scene_->bg_color_.data[1],
                 current_scene_->bg_color_.data[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (auto it = current_scene_->items_.crbegin();
         it != current_scene_->items_.crend(); it++) {
      renderItem(*it);
    }
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

unsigned int Renderer::getVAO(const GameItem *item, bool replace = false) {
  if (!VAO_map_.contains(item) || replace) {
    unsigned int vao;
    switch (item->GetType()) {
    case GameItem::CIRCLE:
      vao = BindCircleVAO(((Circle *)item)->radius_);
      VAO_map_[item] = vao;
      break;
    case GameItem::LINE:
      vao = BindLineVAO(((Line *)item)->x1_, ((Line *)item)->y1_,
                        ((Line *)item)->x2_, ((Line *)item)->y2_);
      VAO_map_[item] = vao;
      break;
    case GameItem::LETTER:
      vao = BindLetterVAO(((Letter *)item)->ch_, ((Letter *)item)->w_,
                          ((Letter *)item)->h_);
      VAO_map_[item] = vao;
      break;
    default:
      break;
    };
  }
  return VAO_map_[item];
}

void Renderer::renderItem(const GameItem *item) {
  if (!item->visible_) {
    return;
  }
  unsigned int shader_program = Shader::Get(item->shader_id_);
  glUseProgram(shader_program);
  switch (item->GetType()) {
  case GameItem::CIRCLE:
    glUniform3f(glGetUniformLocation(shader_program, "color"),
                item->color_.data[0], item->color_.data[1],
                item->color_.data[2]);
    glUniform2f(glGetUniformLocation(shader_program, "pos"), item->x_,
                item->y_);
    glBindVertexArray(getVAO(item));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 200);
    glBindVertexArray(0);
    break;
  case GameItem::LINE:
    glUniform3f(glGetUniformLocation(shader_program, "color"),
                item->color_.data[0], item->color_.data[1],
                item->color_.data[2]);
    glUniform2f(glGetUniformLocation(shader_program, "pos"), 0, 0);
    glBindVertexArray(getVAO(item));
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    break;
  case GameItem::LETTER:
    glUniform2f(glGetUniformLocation(shader_program, "pos"), item->x_,
                item->y_);
    // glUniform3f(glGetUniformLocation(shader_program, "color"),
    //            item->color_.data[0], item->color_.data[1],
    //            item->color_.data[2]);
    glUniform3f(glGetUniformLocation(shader_program, "color"), 1.0f, 1.0f,
                1.0f);
    glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::Get(item->texture_id_));
    glBindVertexArray(getVAO(item));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    break;
  default:
    break;
  };
}
