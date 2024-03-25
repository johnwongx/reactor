#include "EventLoop.h"

#include <iostream>

EventLoop::EventLoop() : ep_(new Epoll()) {}

EventLoop::~EventLoop() {
  delete ep_;
  ep_ = nullptr;
}

bool EventLoop::updateChannel(Channel *chan) {
  return ep_->updateChannel(chan);
}

void EventLoop::run() {
  while (true) {
    std::vector<Channel *> chanList = ep_->loop(5 * 1000);
    if (chanList.empty()) {
      epollTimeoutCallback_(this);
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