#include <iostream>

#include "message_queue.h"

int main() {
  MessageQueue<int> mq(2);
  for (int i = 0; i < 10; i++) {
    int *n = new int(i);
    mq.Push(n);
  }

  int *b;
  while ((b = mq.Pop()) != nullptr) {
    std::cout << *b << ' ';
    delete b;
  }
  std::cout << '\n';

  return 0;
}
