cmake_minimum_required(VERSION 3.12)
project(Simple2DGame)

find_package(OpenGL REQUIRED)
message(STATUS ${OPENGL_VERSION})

include_directories(
  external/glfw/include
  external/glad/include
  external/abseil-cpp
  ${OPENGL_INCLUDE_DIR}
)

add_subdirectory(external)

add_library(
  game
  game.cc
  external/glad/src/glad.c
)
target_link_libraries(
  game
  glfw
  absl::time
  absl::flat_hash_map
  ${OPENGL_gl_LIBRARY}
)

add_executable(
  main
  main.cc
)
target_link_libraries(
  main
  game
)
