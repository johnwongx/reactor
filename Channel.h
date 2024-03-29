/*
用于处理fd发送过来的事件
*/
#pragma once
#include <stdint.h>
#include <sys/epoll.h>

#include <functional>
#include <memory>

class EventLoop;

// 处理事件函数
typedef std::function<bool()> ProcEvtFunc;

// 记录与处理事件
class Channel : public std::enable_shared_from_this<Channel> {
 public:
  Channel(int fd, std::shared_ptr<EventLoop> loop, bool isListen)
      : fd_(fd), loop_(loop), inEpoll_(false) {}
  ~Channel() {}

  int fd() const { return fd_; }

  void setInEpoll(bool in) { inEpoll_ = in; }
  bool inEpoll() const { return inEpoll_; }

  void enableET() { enable(EPOLLET); }
  void disableET() { disable(EPOLLET); }

  void enableRead() { enable(EPOLLIN); }
  void disableRead() { disable(EPOLLIN); }

  void enableWrite() { enable(EPOLLOUT); }
  void disableWrite() { disable(EPOLLOUT); }

  // 将监听的事件更新到epoll中
  void flushEvents();

  uint32_t events() const { return events_; }

  void setREvents(uint32_t events) { rEvents_ = events; }
  uint32_t rEvents() const { return rEvents_; }

  void setInEvtCallbackFunc(ProcEvtFunc fn) { inEvtFunc_ = fn; }
  void setOutEvtCallbackFunc(ProcEvtFunc fn) { outEvtFunc_ = fn; }

  bool handleEvent();

  void setCloseCallback(std::function<void(int)> fn) {
    connCloseCallback_ = fn;
  }

  void setErrorCallback(std::function<void(int)> fn) {
    connErrorCallback_ = fn;
  }

  void Remove();

 private:
  void enable(uint32_t evt) { events_ |= evt; }
  void disable(uint32_t evt) { events_ &= ~evt; }

 private:
  const int fd_;

  std::weak_ptr<EventLoop> loop_;
  bool inEpoll_;

  uint32_t events_;   // 监听的事件
  uint32_t rEvents_;  // 收到的事件

  std::function<bool()> inEvtFunc_;
  std::function<bool()> outEvtFunc_;
  std::function<void(int)> connCloseCallback_;
  std::function<void(int)> connErrorCallback_;
};
typedef std::shared_ptr<Channel> ChannelPtr;