/*
主事件循环，负责epoll调用
*/
#pragma once
#include "Channel.h"
#include "Epoll.h"

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void run();

  bool updateChannel(Channel *chan);

  void setEpollTimeoutCallback(std::function<void(EventLoop *)> fn) {
    epollTimeoutCallback_ = fn;
  }

 private:
  Epoll *ep_;
  std::function<void(EventLoop *)> epollTimeoutCallback_;
};