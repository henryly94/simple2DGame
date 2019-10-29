//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "game_scene.h"
#include "message.pb.h"
#include "message_queue.h"

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

void printByte(char *ptr, size_t size) {
  while (size--) {
    std::cout << (int)(*ptr) << ',';
    ptr++;
  }
  std::cout << '\n';
}

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket, boost::asio::io_context &io,
          GameScene *game_scene)
      : socket_(std::move(socket)), game_scene_(game_scene), io_(io),
        timer_(io, boost::asio::chrono::milliseconds(25)),
        write_timer_(io, boost::asio::chrono::milliseconds(10)) {}

  void start() {
    timer_.async_wait([this](boost::system::error_code) { update(); });
    do_read();
    do_write();
  }

  ~session() { timer_.cancel(); }

private:
  void do_read() {
    auto self(shared_from_this());
    boost::mutex::scoped_lock s_lock(socket_mu_);
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_buf_, max_length),
        boost::bind(&session::lengthHandler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            read_counter_++;
            UpdateProto *proto = new UpdateProto;
            proto->ParseFromArray(read_buf_ + sizeof(size_t),
                                  length - sizeof(size_t));
            mq_.Push(proto);
            do_read();
          }
        });
  }

  size_t lengthHandler(const boost::system::error_code &ec,
                       size_t bytes_transferred) {
    if (bytes_transferred == 0) {
      return sizeof(size_t);
    } else if (bytes_transferred == sizeof(size_t)) {
      return *((size_t *)read_buf_);
    } else {
      return 0;
    }
  }

  void update() {
    auto self(shared_from_this());
    UpdateProto *update = mq_.Pop();
    std::cout << write_counter_ << ' ' << read_counter_ << '\r';
    std::cout.flush();
    while (update != nullptr) {
      // std::cout << update->frame_id() << std::endl;
      game_scene_->Update(*update);
      if (!game_scene_->ball_->visible_) {
        new boost::thread([this, self]() {
          boost::this_thread::sleep_for(boost::chrono::seconds(2));
          game_scene_->RestoreItems();
          delete mq_.PopTillEnd();
        });
      }
      update = mq_.Pop();
    }
    timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(5));
    timer_.async_wait(boost::bind(&session::update, this));
  }

  void do_write() {
    auto self(shared_from_this());
    boost::mutex::scoped_lock s_lock(socket_mu_);
    size_t message_length;
    UpdateProtos updates;
    {
      boost::mutex::scoped_lock lock(game_scene_->mu_);
      updates = game_scene_->protos_;
    }
    updates.set_frame_id(count_++);
    message_length = updates.ByteSizeLong();
    updates.SerializeToArray(write_buf_ + sizeof(size_t), message_length);
    std::copy((char *)&message_length, (char *)&message_length + sizeof(size_t),
              write_buf_);
    message_length += sizeof(size_t);
    boost::asio::async_write(
        socket_, boost::asio::buffer(write_buf_, message_length),
        [this, self](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            write_counter_++;
            write_timer_.expires_from_now(
                boost::asio::chrono::milliseconds(10));
            write_timer_.async_wait(boost::bind(&session::do_write, this));
          }
        });
  }

  tcp::socket socket_;
  unsigned int read_counter_ = 0;
  unsigned int write_counter_ = 0;
  UpdateProtos *protos_;
  MessageQueue<UpdateProto> mq_;
  GameScene *game_scene_;
  boost::asio::steady_timer timer_;
  boost::asio::io_context &io_;
  boost::mutex socket_mu_;
  boost::thread *write_thread_;
  unsigned int count_ = 0;
  boost::asio::steady_timer write_timer_;
  enum { max_length = 10240 };
  char read_buf_[max_length], write_buf_[max_length];
};

class server {
public:
  server(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

private:
  int counter = 0;
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            counter++;
            std::cout << "Accept: " << counter << std::endl;
            std::make_shared<session>(std::move(socket),
                                      acceptor_.get_io_context(), &game_scene_)
                ->start();
          }

          do_accept();
        });
  }

  boost::mutex mu_;
  GameScene game_scene_;
  tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server s(io_context, std::atoi(argv[1]));

    boost::thread_group threads;
    for (int i = 0; i < 4; i++) {
      threads.create_thread(
          boost::bind(&boost::asio::io_context::run, &io_context));
    }

    threads.join_all();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
