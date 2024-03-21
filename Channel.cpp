#include "Channel.h"

#include <cassert>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <sys/epoll.h>

#include "Connector.h"
#include "EventLoop.h"
#include "InetAddress.h"

bool Channel::handleEvent() {
  if (rEvents_ & EPOLLIN) {
    if (!inEvtFunc_)
      return false;

    return inEvtFunc_();
  } else if (rEvents_ & EPOLLOUT) {
    if (!outEvtFunc_)
      return false;
    return outEvtFunc_();
  } else {
    // 其他事件都视为错误
    assert(connErrorCallback_ != nullptr);
    connCloseCallback_(fd_);
    return false;
  }
}

void Channel::flushEvents() { loop_->updateChannel(this); }