#include "controller.h"

#include <boost/thread/thread.hpp>

#include "message.pb.h"

Controller::Controller(GLFWwindow *window)
    : window_(window), io_(), s_(io_), resolver_(io_) {
  boost::asio::connect(s_, resolver_.resolve("127.0.0.1", "1234"));
}

Controller::~Controller() {}

void Controller::ProcessInput(std::string &id) {
  UpdateProto updateProto;
  updateProto.set_id(id);
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
  /*
  if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
    updateProto2.mutable_physics()->set_vy(0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
    updateProto2.mutable_physics()->set_vy(-0.01f);
  }
  if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
    updateProto2.mutable_physics()->set_vx(-0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    updateProto2.mutable_physics()->set_vx(0.01f);
  }
  */
  *update_protos_.add_updates() = std::move(updateProto);
  size_t size = update_protos_.ByteSizeLong();
  update_protos_.SerializeToArray(write_buf_, size);
  boost::asio::write(s_, boost::asio::buffer(write_buf_, size));
  size_t reply_size;
  boost::asio::read(s_, boost::asio::buffer(&reply_size, sizeof(size_t)));
  boost::asio::read(s_, boost::asio::buffer(read_buf_, reply_size));
  log_counter_++;
  if (log_counter_ % 100 == 0) {
    UpdateProtos proto;
    proto.ParseFromArray(read_buf_, reply_size);
    std::cout << proto.DebugString();
    std::cout << '\n';
  }
}

void Controller::ClearUpdates() { update_protos_.clear_updates(); }
