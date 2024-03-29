cmake_minimum_required(VERSION 3.12)
project(Simple2DGame)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenGL REQUIRED)
message(STATUS ${OPENGL_VERSION})

find_package(Boost REQUIRED thread)
set(Boost_USE_STATIC_LIBS ON)

find_package(Protobuf REQUIRED)
#set(Protobuf_USE_STATIC_LIBS ON)
set(PROTOBUF_LIBRARIES
  "/usr/local/lib/libprotobuf.a;-lpthread")
protobuf_generate_cpp(MESSAGE_SRC MESSAGE_HDR message.proto)

include_directories(
  external/glfw/include
  external/glad/include
  external/abseil-cpp
  external/freetype2/include
  ${Boost_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

#external/boost-asio/include

add_subdirectory(external)

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
  message_pb
  ${MESSAGE_SRC}
)
target_link_libraries(
  message_pb
  ${PROTOBUF_LIBRARIES}
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
    glfw
    ${OPENGL_gl_LIBRARY}
)

add_library(
  controller
  controller.cc
)
target_link_libraries(
  controller
  message_pb
  absl::flat_hash_map
  glfw
  ${OPENGL_gl_LIBRARY}
)

add_library(
    game_scene
    game_scene.cc
)
target_link_libraries(
  game_scene
  glfw
  message_pb
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
  controller
  texture
  shader
  glfw
  absl::time
  absl::flat_hash_map
  ${Boost_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  freetype
)

add_executable(
  game_client
  game_client_main.cc
)
target_link_libraries(
  game_client
  game
)

add_executable(
  game_server
  game_server_main.cc
)
target_link_libraries(
  game_server
  message_pb
  game_scene
  absl::flat_hash_map
  ${Boost_LIBRARIES}
)

#add_executable(
#  test_client
#  test_client.cc
#)


add_executable(
  test
  test.cc
)
target_link_libraries(
  test
  ${Boost_LIBRARIES}
)
