#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "scene.h"
#include <absl/container/flat_hash_map.h>
#include <string>

#include <boost/asio.hpp>

#include "message.pb.h"

class Controller {
public:
  Controller(GLFWwindow *window, boost::asio::io_context *io, std::string id);
  ~Controller();

  void ProcessInput();

  void ClearUpdates();

  void Stop();

  GLFWwindow *window_;
  UpdateProtos update_protos_;
  Scene *current_scene_;
  absl::flat_hash_map<std::string, Scene *> scene_map_;
  std::string id_;
  bool live_;

  boost::asio::io_context *io_;
  boost::asio::steady_timer timer_;
  boost::asio::ip::tcp::socket s_;
  boost::asio::ip::tcp::resolver resolver_;
  char write_buf_[1024], read_buf_[1024];
  unsigned int log_counter_ = 0;
};

#endif // CONTROLLER_H_
