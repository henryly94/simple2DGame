#include "controller.h"

#include "message.pb.h"

void printByte(char *ptr, size_t size) {
  while (size--) {
    std::cout << (int)(*ptr) << ',';
    ptr++;
  }
  std::cout << '\n';
}

Controller::Controller(GLFWwindow *window, boost::asio::io_context *io,
                       std::string id, std::string ip, std::string port)
    : window_(window), io_(io), s_(*io_), mq_(10),
      timer_(*io, boost::asio::chrono::milliseconds(10)), resolver_(*io_),
      id_(id), live_(true) {
  boost::asio::connect(s_, resolver_.resolve(ip, port));
  timer_.async_wait([this](boost::system::error_code ec) {
    ProcessInput();
    Read();
    Write();
  });
}

Controller::~Controller() {}

void Controller::ProcessInput() {
  UpdateProto *update_proto = new UpdateProto;
  update_proto->set_id(id_);
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    update_proto->mutable_physics()->set_vy(0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    update_proto->mutable_physics()->set_vy(-0.01f);
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    update_proto->mutable_physics()->set_vx(-0.01f);
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    update_proto->mutable_physics()->set_vx(0.01f);
  }
  update_proto->set_frame_id(counter_++);
  std::cout << counter_ << ' ' << write_counter_ << ' ' << read_counter_
            << "        |" << '\r';
  std::cout.flush();
  mq_.Push(update_proto);
  if (live_) {
    timer_.expires_from_now(boost::asio::chrono::milliseconds(20));
    timer_.async_wait(boost::bind(&Controller::ProcessInput, this));
  }
}

void Controller::Write() {
  auto self(shared_from_this());
  boost::mutex::scoped_lock lock(socket_mu_);
  UpdateProto *update = mq_.Pop();
  if (update == nullptr) {
    boost::asio::steady_timer write_timer(
        *io_, boost::asio::chrono::milliseconds(10));
    write_timer.async_wait(boost::bind(&Controller::Write, this));
  } else {
    write_counter_++;
    size_t size = update->ByteSizeLong();
    std::copy((char *)&size, (char *)&size + sizeof(size_t), write_buf_);
    update->SerializeToArray(write_buf_ + sizeof(size_t), size);
    boost::asio::async_write(
        s_, boost::asio::buffer(write_buf_, size + sizeof(size_t)),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          Write();
        });
    delete update;
  }
}

void Controller::Read() {
  auto self(shared_from_this());
  boost::mutex::scoped_lock lock(socket_mu_);
  boost::asio::async_read(
      s_, boost::asio::buffer(read_buf_, max_length_),
      boost::bind(&Controller::lengthHandler, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
          // read_counter_++;
          UpdateProtos updates;
          updates.ParseFromArray(read_buf_ + sizeof(size_t),
                                 length - sizeof(size_t));
          read_counter_ = updates.frame_id();
          // std::cout << updates.DebugString();
          if (updates.frame_id() > current_frame_id_) {
            current_frame_id_ = updates.frame_id();
            current_scene_->Update(updates);
          }
          Read();
        }
      });
}

void Controller::Update() {
  /*
  UpdateProtos *tmp = mqs_.Pop(), *updates = nullptr;
  if (tmp != nullptr) {
    while (tmp != nullptr) {
      if (updates != nullptr) {
        delete updates;
      }
      updates = tmp;
      tmp = mqs_.Pop();
    }
    current_scene_->Update(*updates);
  }
  */
  UpdateProtos *updates = mqs_.PopTillEnd();
  if (updates != nullptr) {
    current_scene_->Update(*updates);
  }
  if (live_) {
    boost::asio::steady_timer update_timer(
        *io_, boost::asio::chrono::milliseconds(10));
    update_timer.async_wait(boost::bind(&Controller::Update, this));
  }
}

size_t Controller::lengthHandler(const boost::system::error_code &ec,
                                 size_t bytes_transferred) {
  if (bytes_transferred == 0) {
    return sizeof(size_t);
  } else if (bytes_transferred == sizeof(size_t)) {
    return *((size_t *)read_buf_);
  } else {
    return 0;
  }
}

void Controller::Stop() { live_ = false; }
