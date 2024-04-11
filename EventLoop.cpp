#include "EventLoop.h"

#include <string.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <iostream>

#include "Connector.h"

void ResetTimer(int tfd, int sec) {
  itimerspec timeout;
  memset(&timeout, 0, sizeof(itimerspec));
  timeout.it_value.tv_sec = sec;
  timeout.it_value.tv_nsec = 0;
  timerfd_settime(tfd, 0, &timeout, 0);
}

EventLoop::EventLoop(uint32_t timeoutCheckInter)
    : ep_(std::make_unique<Epoll>()),
      threadId_(-1),
      stop_(false),
      weakupChan_(new Channel(eventfd(0, EFD_NONBLOCK), *this)),
      timeoutCheckInter_(timeoutCheckInter) {
  weakupChan_->setInEvtCallbackFunc(std::bind(&EventLoop::HandleTasks, this));
  weakupChan_->enableRead();
  weakupChan_->enableET();
  weakupChan_->flushEvents();

  if (timeoutCheckInter_ != -1) {
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    timerChan_ = std::make_unique<Channel>(tfd, *this);
    timerChan_->setInEvtCallbackFunc(std::bind(&EventLoop::HandleTimer, this));
    timerChan_->enableRead();
    timerChan_->enableET();
    timerChan_->flushEvents();
    ResetTimer(tfd, timeoutCheckInter_);
  }
}
EventLoop::~EventLoop() { Stop(); }

void EventLoop::Run() {
  threadId_ = syscall(SYS_gettid);
  printf("EventLoop::run() at thread(%ld).\n", threadId_);

  while (!stop_) {
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
void EventLoop::Stop() {
  if (stop_) return;
  stop_ = true;
  WeakupTaskProcess();
}

void EventLoop::WeakupTaskProcess() {
  uint64_t u = 1;
  ssize_t len = write(weakupChan_->fd(), &u, sizeof(u));
  if (len == -1) {
    perror("EventLoop::WeakupTaskProcess() failed:");
  }
}

bool EventLoop::HandleTasks() {
  uint64_t u = 0;
  ssize_t len = read(weakupChan_->fd(), &u, sizeof(uint64_t));
  if (len == -1) {
    perror("EventLoop::ProcessTasks() read failed:");
    return false;
  }

  std::lock_guard<std::mutex> guard(taskMtx_);
  TaskFunc fn;
  while (!tasks_.empty()) {
    fn = std::move(tasks_.front());
    tasks_.pop();

    fn();
  }
  return true;
}

bool EventLoop::HandleTimer() {
  // 重设计时
  ResetTimer(timerChan_->fd(), timeoutCheckInter_);

  for (auto it = connList_.begin(); it != connList_.end();) {
    ConnectorPtr conn = it->second.lock();
    if (!conn) {
      std::lock_guard<std::mutex> guard(connMtx_);
      it = connList_.erase(it);
      continue;
    }

    if (conn->IsTimeout()) {
      {
        std::lock_guard<std::mutex> guard(connMtx_);
        it = connList_.erase(it);
      }
      conn->OnClose();
    } else {
      it++;
    }
  }
  return true;
}

void EventLoop::AddConnector(std::shared_ptr<Connector> conn) {
  std::lock_guard<std::mutex> guard(connMtx_);
  connList_[conn->fd()] = conn;
}