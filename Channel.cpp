#include "Channel.h"

#include <stdio.h>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cassert>

#include "Connector.h"
#include "EventLoop.h"
#include "InetAddress.h"

bool Channel::handleEvent() {
  if (rEvents_ & EPOLLIN) {
    if (!inEvtFunc_) return false;

    return inEvtFunc_();
  } else if (rEvents_ & EPOLLOUT) {
    if (!outEvtFunc_) return false;
    return outEvtFunc_();
  } else {
    // 其他事件都视为错误
    assert(connErrorCallback_ != nullptr);
    connErrorCallback_(fd_);
    return false;
  }
}
void Channel::flushEvents() { loop_.updateChannel(*this); }
void Channel::Remove() { loop_.RemoveChannel(*this); }