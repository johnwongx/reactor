/*
执行事件循环，以及向epoll中添加fd
*/
#pragma once
#include <sys/epoll.h>

#include <memory>
#include <vector>

class Channel;

class Epoll {
 public:
  Epoll();
  ~Epoll();

  std::vector<Channel*> Loop(int timeout);
  bool UpdateChannel(Channel& chan);
  void RemoveChannel(Channel& chan);

 private:
  int epollfd_;

  static const int MaxEventSize = 100;
  epoll_event evts_[MaxEventSize];
};