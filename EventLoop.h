/*
epoll 的操作者，并提供唤醒与清除空闲连接的功能。
事件发生后，触发事件处理操作。

主事件循环，负责epoll调用
*/
#pragma once
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include "Channel.h"
#include "Epoll.h"

class Connector;

class EventLoop {
 public:
  // timeoutCheckInter:连接超时检查间隔， -1时将不检查
  EventLoop(uint32_t timeoutCheckInter = 5);
  ~EventLoop();

  void Run();
  void Stop();

  bool updateChannel(Channel& chan) { return ep_->UpdateChannel(chan); }
  void RemoveChannel(Channel& chan) { ep_->RemoveChannel(chan); }

  void setEpollTimeoutCallback(std::function<void(EventLoop&)> fn) {
    epollTimeoutCallback_ = fn;
  }

  int64_t GetThreadId() const { return threadId_; }

  typedef std::function<void()> TaskFunc;
  void PushTask(TaskFunc fn) {
    {
      std::lock_guard guard(taskMtx_);
      tasks_.emplace(fn);
    }
    WeakupTaskProcess();
  }

  void AddConnector(std::shared_ptr<Connector> conn);

 private:
  void WeakupTaskProcess();
  bool HandleTasks();

  bool HandleTimer();

 private:
  std::unique_ptr<Epoll> ep_;
  std::function<void(EventLoop&)> epollTimeoutCallback_;
  int64_t threadId_;

  std::atomic_bool stop_;

  std::mutex taskMtx_;
  std::queue<TaskFunc> tasks_;
  std::unique_ptr<Channel> weakupChan_;

  uint32_t timeoutCheckInter_;          // 超时检查间隔
  std::unique_ptr<Channel> timerChan_;  // 用于检查空闲连接
  std::mutex connMtx_;
  std::map<int, std::weak_ptr<Connector>> connList_;
};