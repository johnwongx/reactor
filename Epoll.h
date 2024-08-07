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

  // 等待事件触发，处理将事件组装为channel对象，然后返回
  std::vector<Channel*> Loop(int timeout);

  // 更新监听的channel 对象状态，如果对象没有在epoll 中，
  // 就添加进去，如果在里边，就修改监听事件
  bool UpdateChannel(Channel& chan);

  // 从epoll 中移除
  void RemoveChannel(Channel& chan);

 private:
  int epollfd_;

  static const int MaxEventSize = 100;
  epoll_event evts_[MaxEventSize];
};