#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/string_view.h>
#include <string>

class Texture {
public:
  using registry_type = absl::flat_hash_map<std::string, unsigned int>;

  static void Bind(std::string name, GLvoid *data, int width, int height,
                   GLenum data_format, GLint texture_format);

  static unsigned int Get(std::string name);

private:
  static registry_type &registry() {
    static registry_type _;
    return _;
  }
};

#endif // TEXTURE_H_
