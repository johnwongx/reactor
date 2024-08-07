#include "Epoll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cassert>
#include <iostream>

#include "Channel.h"

Epoll::Epoll() {
  epollfd_ = epoll_create(1);
  if (epollfd_ == -1) {
    printf("epoll_create() failed(%d)!\n", errno);
    exit(-1);
  }
}

Epoll::~Epoll() { close(epollfd_); }

std::vector<Channel*> Epoll::Loop(int timeout) {
  memset(evts_, 0, sizeof(evts_));

  int infds = epoll_wait(epollfd_, evts_, MaxEventSize, timeout);
  if (infds < 0) {
    // 程序错误或者信号中断
    perror("epoll_wait() failed!");
    exit(-1);
  } else if (infds == 0) {
    return {};
  }

  std::vector<Channel*> res;
  res.reserve(infds);
  for (int i = 0; i < infds; i++) {
    Channel* chan = static_cast<Channel*>(evts_[i].data.ptr);
    // 检测到的事件，如 EPOLLIN, EPOLLOUT,EPOLLERR等
    // 只有用户委托epoll检测某个事件时，发生的事件才会被检测到
    chan->setREvents(evts_[i].events);
    res.push_back(chan);
  }
  return res;
}

bool Epoll::UpdateChannel(Channel& chan) {
  epoll_event evt;
  evt.data.ptr = &chan;
  evt.events = chan.events();

  if (chan.inEpoll()) {
    int iret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, chan.fd(), &evt);
    if (iret != 0) {
      perror("modify event to epoll failed!");
      return false;
    }
    return true;
  } else {
    int iret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, chan.fd(), &evt);
    if (iret != 0) {
      perror("add event to epoll failed!");
      return false;
    }
    chan.setInEpoll(true);
    return true;
  }
}

void Epoll::RemoveChannel(Channel& chan) {
  assert(chan.inEpoll());
  std::cout << "Epoll::RemoveChannel()" << std::endl;
  int iret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, chan.fd(), nullptr);
  if (iret != 0) {
    perror("remove channel failed!");
    return;
  }
}