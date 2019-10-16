#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

void Shader::Bind(absl::string_view name, absl::string_view vs_path,
                  absl::string_view fs_path) {
  std::string vsCode, fsCode;
  std::ifstream vsFile, fsFile;
  vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vsFile.open(vs_path.data());
    fsFile.open(fs_path.data());
    std::stringstream vss, fss;
    vss << vsFile.rdbuf();
    fss << fsFile.rdbuf();
    vsFile.close();
    fsFile.close();
    vsCode = vss.str();
    fsCode = fss.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "Shader file reading error: " << e.what() << std::endl;
  }

  const char *vsSource = vsCode.c_str();
  const char *fsSource = fsCode.c_str();
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vsSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fsSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
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
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  registry()[name] = shaderProgram;
}

unsigned int Shader::Get(absl::string_view name) {
  auto it = registry().find(name);
  if (it == registry().end()) {
    return 0;
  }
  return it->second;
}
