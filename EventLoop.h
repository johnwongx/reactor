/*
主事件循环，负责epoll调用
*/
#pragma once
#include <memory>

#include "Channel.h"
#include "Epoll.h"

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void run();

  bool updateChannel(Channel& chan) { return ep_->UpdateChannel(chan); }
  void RemoveChannel(Channel& chan) { ep_->RemoveChannel(chan); }

  void setEpollTimeoutCallback(std::function<void(EventLoop&)> fn) {
    epollTimeoutCallback_ = fn;
  }

 private:
  std::unique_ptr<Epoll> ep_;
  std::function<void(EventLoop&)> epollTimeoutCallback_;
};