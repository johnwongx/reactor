/*
主事件循环，负责epoll调用
*/
#pragma once
#include <memory>

#include "Channel.h"
#include "Epoll.h"

class EventLoop : public std::enable_shared_from_this<EventLoop> {
 public:
  EventLoop();
  ~EventLoop();

  void run();

  bool updateChannel(ChannelPtr chan);
  void RemoveChannel(ChannelPtr chan);

  void setEpollTimeoutCallback(
      std::function<void(std::shared_ptr<EventLoop>)> fn) {
    epollTimeoutCallback_ = fn;
  }

 private:
  EpollPtr ep_;
  std::function<void(std::shared_ptr<EventLoop>)> epollTimeoutCallback_;
};

typedef std::shared_ptr<EventLoop> EventLoopPtr;