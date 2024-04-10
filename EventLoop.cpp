#include "EventLoop.h"

#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>

EventLoop::EventLoop()
    : ep_(std::make_unique<Epoll>()),
      threadId_(-1),
      weakupChan_(new Channel(eventfd(0, EFD_NONBLOCK), *this)) {
  weakupChan_->setInEvtCallbackFunc(std::bind(&EventLoop::ProcessTasks, this));
  weakupChan_->enableRead();
  weakupChan_->enableET();
  updateChannel(*weakupChan_);
}

EventLoop::~EventLoop() {}

void EventLoop::run() {
  threadId_ = syscall(SYS_gettid);
  printf("EventLoop::run() at thread(%ld).\n", threadId_);

  while (true) {
    std::vector<Channel *> chanList = ep_->Loop(5 * 1000);
    if (chanList.empty()) {
      epollTimeoutCallback_(*this);
      continue;
    }

    for (const auto &chanItr : chanList) {
      if (!chanItr->handleEvent()) {
        std::cout << "handle event failed!" << std::endl;
        continue;
      }
    }
  }
}

void EventLoop::WeakupTaskProcess() {
  uint64_t u = 1;
  ssize_t len = write(weakupChan_->fd(), &u, sizeof(u));
  if (len == -1) {
    perror("EventLoop::WeakupTaskProcess() failed:");
  }
}

bool EventLoop::ProcessTasks() {
  uint64_t u = 0;
  ssize_t len = read(weakupChan_->fd(), &u, sizeof(uint64_t));
  if (len == -1) {
    perror("EventLoop::ProcessTasks() read failed:");
    return false;
  }

  std::lock_guard<std::mutex> guard(mtx_);
  TaskFunc fn;
  while (!tasks_.empty()) {
    fn = std::move(tasks_.front());
    tasks_.pop();

    fn();
  }
  return true;
}