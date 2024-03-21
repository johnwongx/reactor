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

private:
  Epoll *ep_;
};