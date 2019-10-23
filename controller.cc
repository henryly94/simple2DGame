#include "controller.h"

#include "message.pb.h"

Controller::Controller(GLFWwindow *window, boost::asio::io_context *io,
                       std::string id, std::string ip, std::string port)
    : window_(window), io_(io), s_(*io_),
      timer_(*io, boost::asio::chrono::milliseconds(33)),
      read_timer_(*io, boost::asio::chrono::milliseconds(33)), resolver_(*io_),
      id_(id), live_(true) {
  boost::asio::connect(s_, resolver_.resolve(ip, port));
  timer_.async_wait(boost::bind(&Controller::ProcessInput, this));
  read_timer_.async_wait(boost::bind(&Controller::Read, this));
  t_ = new boost::thread([this]() {
    while (true) {
      UpdateProtos *updates;
      while ((updates = mq_.Pop()) != nullptr) {
        current_scene_->Update(*updates);
        delete updates;
      }
      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    }
  });
}

Controller::~Controller() {}

void Controller::ProcessInput() {
  UpdateProto update_proto;
  update_proto.set_id(id_);
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    update_proto.mutable_physics()->set_vy(0.02f);
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    update_proto.mutable_physics()->set_vy(-0.02f);
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    update_proto.mutable_physics()->set_vx(-0.02f);
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    update_proto.mutable_physics()->set_vx(0.02f);
  }

  size_t size = update_proto.ByteSizeLong();
  update_proto.SerializeToArray(write_buf_, size);
  boost::asio::write(s_, boost::asio::buffer(write_buf_, size));
  if (live_) {
    timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(33));
    timer_.async_wait(boost::bind(&Controller::ProcessInput, this));
  }
}

void Controller::Read() {
  size_t reply_size;
  boost::asio::read(s_, boost::asio::buffer(&reply_size, sizeof(size_t)));
  boost::asio::read(s_, boost::asio::buffer(read_buf_, reply_size));
  UpdateProtos *protos = new UpdateProtos;
  protos->ParseFromArray(read_buf_, reply_size);
  mq_.Push(protos);

  if (live_) {
    timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(33));
    timer_.async_wait(boost::bind(&Controller::Read, this));
  }
}

void Controller::ClearUpdates() { update_protos_.clear_updates(); }

void Controller::Stop() { live_ = false; }
