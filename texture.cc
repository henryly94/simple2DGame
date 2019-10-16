#include "texture.h"
#include <iostream>

void Texture::Bind(std::string name, GLvoid *data, int width, int height,
                   GLenum data_format, GLint texture_format) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (data_format == GL_RGBA) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, 0, data_format,
               GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  registry()[name] = texture;
}

unsigned int Texture::Get(std::string name) {
  auto it = registry().find(name);
  if (it == registry().end()) {
    return 0;
  }
  return it->second;
}
