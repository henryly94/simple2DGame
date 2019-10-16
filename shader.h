#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/string_view.h>
#include <string>

class Shader {
public:
  using registry_type = absl::flat_hash_map<std::string, unsigned int>;
  static void Bind(absl::string_view name, absl::string_view vs_path,
                   absl::string_view fs_path);

  static unsigned int Get(absl::string_view name);

private:
  static registry_type &registry() {
    static registry_type _;
    return _;
  }
};

#endif // SHADER_H_
