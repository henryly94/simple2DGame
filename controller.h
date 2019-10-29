#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "scene.h"
#include <absl/container/flat_hash_map.h>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "message_queue.h"

#include "message.pb.h"

class Controller : public std::enable_shared_from_this<Controller> {
public:
  Controller(GLFWwindow *window, boost::asio::io_context *io, std::string id,
             std::string ip, std::string port);
  ~Controller();

  void ProcessInput();

  void Read();

  size_t lengthHandler(const boost::system::error_code &ec,
                       size_t bytes_transferred);

  void Write();

  void Update();

  void Stop();

  GLFWwindow *window_;
  UpdateProtos update_protos_;
  Scene *current_scene_;
  absl::flat_hash_map<std::string, Scene *> scene_map_;
  std::string id_;
  bool live_;
  unsigned int counter_ = 0;
  unsigned int write_counter_ = 0;
  unsigned int read_counter_ = 0;
  unsigned int current_frame_id_ = 0;

  MessageQueue<UpdateProto> mq_;
  MessageQueue<UpdateProtos> mqs_;

  boost::mutex socket_mu_;
  boost::asio::io_context *io_;
  boost::asio::steady_timer timer_;
  boost::asio::ip::tcp::socket s_;
  boost::asio::ip::tcp::resolver resolver_;
  size_t message_length_;
  const static size_t max_length_ = 10240;
  char write_buf_[max_length_], read_buf_[max_length_];
};

#endif // CONTROLLER_H_
