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
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket, GameScene *game_scene)
      : socket_(std::move(socket)), game_scene_(game_scene) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            UpdateProto proto;
            proto.ParseFromArray(data_, length);
            // std::cout << ">> " << proto.DebugString() << std::endl;
            do_write(std::move(proto));
          }
        });
  }

  void do_write(UpdateProto &&proto) {
    auto self(shared_from_this());
    {
      boost::mutex::scoped_lock(game_scene_->mu_);
      game_scene_->Update(proto);
      message_length_ = game_scene_->protos_.ByteSizeLong();
      game_scene_->protos_.SerializeToArray(data_, message_length_);
    }
    // if (protos_->updates_size() == 2) {
    // protos_->SerializeToArray(data_, length);
    // std::cout << ">>" << game_scene_->protos_.DebugString();
    boost::asio::async_write(
        socket_, boost::asio::buffer(&message_length_, sizeof(size_t)),
        [this, self](boost::system::error_code ec, std::size_t) {
          boost::asio::async_write(
              socket_, boost::asio::buffer(data_, message_length_),
              [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                  do_read();
                }
              });
        });
    //} else {
    //  do_read();
    //}
  }

  tcp::socket socket_;
  UpdateProtos *protos_;
  GameScene *game_scene_;
  boost::mutex mu_;
  boost::thread *t_;
  size_t message_length_;
  enum { max_length = 1024 };
  char data_[max_length];
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
            std::make_shared<session>(std::move(socket), &game_scene_)->start();
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

    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
