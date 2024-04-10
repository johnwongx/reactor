/*
主事件循环，负责epoll调用
*/
#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

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

  int64_t GetThreadId() const { return threadId_; }

  typedef std::function<void()> TaskFunc;
  void PushTask(TaskFunc fn) {
    {
      std::lock_guard guard(mtx_);
      tasks_.emplace(fn);
    }
    WeakupTaskProcess();
  }

 private:
  void WeakupTaskProcess();
  bool ProcessTasks();

 private:
  std::unique_ptr<Epoll> ep_;
  std::function<void(EventLoop&)> epollTimeoutCallback_;
  int64_t threadId_;

  std::mutex mtx_;
  std::queue<TaskFunc> tasks_;
  std::unique_ptr<Channel> weakupChan_;
};