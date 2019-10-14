#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#include "factory.h"
#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
class GameItem {
public:
  using factory = Factory<GameItem>;
  enum Type {
    CIRCLE,
    LINE,
    LETTER,
  };

  enum Group { SCENE, BACKGROUND, PLAYER };

  GameItem()
      : x_(0), y_(0), vx_(0), vy_(0), ax_(0), ay_(0), m_(1), r_(1), g_(1),
        b_(1), visible_(true), exist_(true), group_(BACKGROUND) {}
  virtual void Draw() const = 0;
  virtual Type GetType() const = 0;

  int shader_program() const {
    static int _ = []() {
      const char *vertexShaderSource =
          "#version 330 core\n"
          "layout (location = 0) in vec2 aPos;\n"
          "uniform vec2 pos;\n"
          "void main()\n"
          "{\n"
          "   gl_Position = vec4(aPos.x + pos.x, aPos.y + pos.y, 0.0, 1.0);\n"
          "}\0";
      const char *fragmentShaderSource = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "uniform vec3 color;\n"
                                         "void main()\n"
                                         "{\n"
                                         "   FragColor = vec4(color, 1.0f);\n"
                                         "}\n\0";

      int vertexShader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
      glCompileShader(vertexShader);
      // check for shader compile errors
      int success;
      char infoLog[512];
      glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
      }
      // fragment shader
      int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
      glCompileShader(fragmentShader);
      // check for shader compile errors
      glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
      }
      // link shaders
      int shaderProgram = glCreateProgram();
      glAttachShader(shaderProgram, vertexShader);
      glAttachShader(shaderProgram, fragmentShader);
      glLinkProgram(shaderProgram);
      // check for linking errors
      glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
      }
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      return shaderProgram;
    }();
    return _;
  }

  float x_, y_;
  float vx_, vy_;
  float ax_, ay_;
  float m_;
  float r_, g_, b_;
  Group group_;
  bool visible_;
  bool exist_;
};

#endif // GAME_ITEM_H_
