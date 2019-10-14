#ifndef LETTER_H_
#define LETTER_H_

#include <iostream>

#include "game_item.h"
#include <ft2build.h>

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include FT_FREETYPE_H

class Letter : public GameItem {
public:
  using factory = Factory<Letter>;
  Letter(char ch, float x, float y, float w, float h) : ch_(ch), w_(w), h_(h) {
    x_ = x;
    y_ = y;
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    float vertices[] = {-w_ / 2, -h_ / 2, 0.0f,    0.0f,   w_ / 2, -h_ / 2,
                        1.0f,    0.0f,    w_ / 2,  h_ / 2, 1.0f,   1.0f,
                        -w_ / 2, -h_ / 2, 0.0f,    0.0f,   w_ / 2, h_ / 2,
                        1.0f,    1.0f,    -w_ / 2, h_ / 2, 0.0f,   1.0f};
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    UpdateChar();
  }

  void Draw() const override {
    glUseProgram(letter_shader_program());
    glUniform2f(glGetUniformLocation(letter_shader_program(), "pos"), x_, y_);
    glUniform3f(glGetUniformLocation(letter_shader_program(), "fontColor"), r_,
                g_, b_);
    glUniform1i(glGetUniformLocation(letter_shader_program(), "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
  }

  Type GetType() const override { return LETTER; }

  void UpdateChar() {
    FT_Load_Char(face(), ch_, FT_LOAD_RENDER);
    FT_GlyphSlot &slot = face()->glyph; // 输出结果在 slot->bitmap
    auto &bitmap = slot->bitmap;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.rows, 0, GL_RED,
                 GL_UNSIGNED_BYTE, bitmap.buffer);
    // 设置纹理选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
  char ch_;
  float w_, h_;

private:
  GLuint texture_;
  GLuint VAO_, VBO_;

  static FT_Face face() {
    static FT_Face face = []() {
      FT_Library library;         // 声明了Lib
      FT_Init_FreeType(&library); // 初始化 返回0为成功:w
      FT_Face face;
      int error = FT_New_Face(library, "../resource/Consolas.ttf", 0, &face);
      std::cout << error << std::endl;
      FT_Set_Pixel_Sizes(face, 0, 128); // 设立字体为16px
      return face;
    }();
    return face;
  }

  static unsigned int letter_shader_program() {
    static unsigned int id = []() {
      const char *vs =
          "#version 330 core\n"
          "layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
          "out vec2 TexCoords;\n"
          "uniform vec2 pos;\n"
          "\n"
          "void main()\n"
          "{\n"
          "        gl_Position = vec4(vertex.x + pos.x, vertex.y + pos.y, 0.0, "
          "1.0);\n"
          "        TexCoords = vec2(vertex.z, 1-vertex.w);\n"
          "}\n";

      const char *fs = "#version 330 core\n"
                       "in vec2 TexCoords;\n"
                       "out vec4 FragColor;\n"
                       "\n"
                       "uniform sampler2D texture1;\n"
                       "uniform vec3 fontColor;\n"
                       "\n"
                       "void main()\n"
                       "{    \n"
                       "      FragColor = vec4(fontColor,  texture(texture1, "
                       "TexCoords).r);\n"
                       "}\n";
      int vertexShader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertexShader, 1, &vs, NULL);
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
      glShaderSource(fragmentShader, 1, &fs, NULL);
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
    return id;
  }
};

#endif // LETTER_H_
