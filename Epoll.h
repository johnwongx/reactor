/*
执行事件循环，以及向epoll中添加fd
*/
#pragma once
#include <sys/epoll.h>

#include <memory>
#include <vector>

#include "Channel.h"

class Epoll {
 public:
  Epoll();
  ~Epoll();

  std::vector<ChannelPtr> loop(int timeout);
  bool updateChannel(ChannelPtr chan);

 private:
  int epollfd_;

  static const int MaxEventSize = 100;
  epoll_event evts_[MaxEventSize];
};

typedef std::shared_ptr<Epoll> EpollPtr;