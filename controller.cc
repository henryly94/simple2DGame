#include "controller.h"

#include <boost/thread/thread.hpp>

#include "message.pb.h"

Controller::Controller(GLFWwindow *window, boost::asio::io_context *io,
                       std::string id)
    : window_(window), io_(io), s_(*io_),
      timer_(*io, boost::asio::chrono::milliseconds(10)), resolver_(*io_),
      id_(id), live_(true) {
  boost::asio::connect(s_, resolver_.resolve("127.0.0.1", "1234"));
  timer_.async_wait(boost::bind(&Controller::ProcessInput, this));
}

Controller::~Controller() {}

void Controller::ProcessInput() {
  update_protos_.clear_updates();
  UpdateProto updateProto;
  updateProto.set_id(id_);
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    updateProto.mutable_physics()->set_vy(0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    updateProto.mutable_physics()->set_vy(-0.01f);
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    updateProto.mutable_physics()->set_vx(-0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    updateProto.mutable_physics()->set_vx(0.01f);
  }
  *update_protos_.add_updates() = std::move(updateProto);
  /*
  size_t size = update_protos_.ByteSizeLong();
  update_protos_.SerializeToArray(write_buf_, size);
  boost::asio::async_write(s_, boost::asio::buffer(write_buf_, size),
                           [](boost::system::error_code, std::size_t) {
                             std::cout << "Send success\n";
                           });
  size_t reply_size;
  boost::asio::async_read(s_, boost::asio::buffer(&reply_size, sizeof(size_t)),
                          [&](boost::system::error_code, std::size_t) {
                            boost::asio::async_read(
                                s_, boost::asio::buffer(read_buf_, reply_size),
                                [&](boost::system::error_code, std::size_t) {
                                  UpdateProtos protos;
                                  protos.ParseFromArray(read_buf_, reply_size);
                                  log_counter_++;
                                  if (log_counter_ % 100 == 0) {
                                    std::cout << protos.DebugString();
                                    std::cout << '\n';
                                  }
                                  boost::mutex::scoped_lock(
                                      current_scene_->mu_);
                                  current_scene_->Update(protos);
                                });
                          });
  */
  size_t size = update_protos_.ByteSizeLong();
  update_protos_.SerializeToArray(write_buf_, size);
  boost::asio::write(s_, boost::asio::buffer(write_buf_, size));
  // size_t reply_size;
  // boost::asio::read(s_, boost::asio::buffer(&reply_size, sizeof(size_t)));
  boost::asio::read(s_, boost::asio::buffer(read_buf_, size));
  UpdateProtos protos;
  protos.ParseFromArray(read_buf_, size);
  log_counter_++;
  if (log_counter_ % 100 == 0) {
    std::cout << protos.DebugString();
    std::cout << '\n';
  }
  boost::mutex::scoped_lock(current_scene_->mu_);
  current_scene_->Update(protos);
  if (live_) {
    timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(10));
    timer_.async_wait(boost::bind(&Controller::ProcessInput, this));
  }
}

void Controller::ClearUpdates() { update_protos_.clear_updates(); }

void Controller::Stop() { live_ = false; }
