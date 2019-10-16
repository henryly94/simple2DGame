cmake_minimum_required(VERSION 3.12)
project(Simple2DGame)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenGL REQUIRED)
message(STATUS ${OPENGL_VERSION})

add_subdirectory(external)

include_directories(
  external/glfw/include
  external/glad/include
  external/abseil-cpp
  external/freetype2/include
  ${OPENGL_INCLUDE_DIR}
)

add_library(
    shader
    shader.cc
)
target_link_libraries(
    shader
    absl::flat_hash_map
    absl::strings
    glfw
    ${OPENGL_gl_LIBRARY}
)

add_library(
    texture
    texture.cc
)
target_link_libraries(
    texture
    absl::flat_hash_map
    absl::strings
    glfw
    ${OPENGL_gl_LIBRARY}
)

add_library(
    renderer
    renderer.cc
)
target_link_libraries(
    renderer
    absl::strings
    absl::flat_hash_map
)


add_library(
    game_scene
    game_scene.cc
)
target_link_libraries(
  game_scene
  glfw
  ${OPENGL_gl_LIBRARY}
  freetype
)

add_library(
  game
  game.cc
  external/glad/src/glad.c
)
target_link_libraries(
  game
  game_scene
  renderer
  texture
  shader
  glfw
  absl::time
  absl::flat_hash_map
  ${OPENGL_gl_LIBRARY}
  freetype
)

add_executable(
  main
  main.cc
)
target_link_libraries(
  main
  game
)
