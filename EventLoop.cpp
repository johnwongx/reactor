#include "EventLoop.h"

#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>

EventLoop::EventLoop() : ep_(std::make_shared<Epoll>()) {}

EventLoop::~EventLoop() {}

bool EventLoop::updateChannel(ChannelPtr chan) {
  return ep_->UpdateChannel(chan);
}

void EventLoop::run() {
  // printf("EventLoop::run() at thread(%ld).\n", syscall(SYS_gettid));

  while (true) {
    std::vector<ChannelPtr> chanList = ep_->Loop(5 * 1000);
    if (chanList.empty()) {
      epollTimeoutCallback_(shared_from_this());
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

void EventLoop::RemoveChannel(ChannelPtr chan) { ep_->RemoveChannel(chan); }