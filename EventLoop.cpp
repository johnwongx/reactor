#include "EventLoop.h"

#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>

EventLoop::EventLoop() : ep_(std::make_unique<Epoll>()) {}

EventLoop::~EventLoop() {}

void EventLoop::run() {
  // printf("EventLoop::run() at thread(%ld).\n", syscall(SYS_gettid));

  while (true) {
    std::vector<Channel *> chanList = ep_->Loop(5 * 1000);
    if (chanList.empty()) {
      epollTimeoutCallback_(*this);
      continue;
    }

    for (const auto &chanItr : chanList) {
      if (!chanItr->handleEvent()) {
        std::cout << "handle event failed!" << std::endl;
        continue;
      }
    }
  }
}