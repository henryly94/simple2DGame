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
