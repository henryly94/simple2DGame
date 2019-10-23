#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <boost/thread/mutex.hpp>

#include <algorithm>

template <typename Message> class MessageQueue {
public:
  MessageQueue(size_t queue_size = 1024)
      : queue_size_(queue_size), current_read_(0), current_write_(0),
        current_length_(0) {
    queue_ = new Message *[queue_size];
  }

  // Try to get a message from queue, return nullptr if not available.
  Message *Pop() {
    boost::mutex::scoped_lock(mu_);
    if (current_read_ == current_write_) {
      return nullptr;
    }
    Message *ret = queue_[current_read_];
    current_read_ = (current_read_ + 1) % queue_size_;
    current_length_--;
    return ret;
  }

  void Push(Message *message_ptr) {
    boost::mutex::scoped_lock(mu_);
    if (current_length_ == queue_size_) {
      double_size();
    }
    current_length_++;
    queue_[current_write_++] = message_ptr;
  }

private:
  void double_size() {
    Message **new_queue_ = new Message *[queue_size_ * 2];
    if (current_read_ <= current_write_) {
      std::copy(queue_ + current_read_, queue_ + current_write_, new_queue_);
    } else {
      std::copy(queue_ + current_read_, queue_ + queue_size_ - 1, new_queue_);
      std::copy(queue_, queue_ + current_write_,
                new_queue_ + (queue_size_ - current_read_ + 1));
    }
    delete[] queue_;
    queue_ = new_queue_;
    queue_size_ *= 2;
    current_read_ = 0;
    current_write_ = current_length_;
  }

  Message **queue_;
  boost::mutex mu_;
  size_t queue_size_;
  size_t current_read_;
  size_t current_write_;
  size_t current_length_;
};

#endif // MESSAGE_QUEUE_H
