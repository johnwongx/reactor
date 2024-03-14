#include "Epoll.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

Epoll::Epoll()
{
    epollfd_ = epoll_create(1);
    if (epollfd_ == -1)
    {
        printf("epoll_create() failed(%d)!\n", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

std::vector<epoll_event> Epoll::loop(int timeout)
{
    memset(evts_, 0, sizeof(evts_));

    int infds = epoll_wait(epollfd_, evts_, MaxEventSize, timeout);
    if(infds < 0)
    {
        perror("epoll_wait() failed!");
        exit(-1);
    }
    else if (infds == 0)
    {
        std::cout << "epoll_wait() timeout!" << std::endl;
        return {};
    }

    std::vector<epoll_event> res;
    res.reserve(infds);
    for (int i = 0; i < infds; i++)
    {
        res.push_back(evts_[i]);
    }
    return res;
}

bool Epoll::addFd(int fd, int op)
{
    epoll_event evt;
    evt.data.fd = fd;
    evt.events = op;
    int iret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &evt);
    if (iret != 0)
    {
        perror("add event to epoll failed!");
        return false;
    }
    return true;
}