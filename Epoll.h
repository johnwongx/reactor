#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll
{
public:
    Epoll();
    ~Epoll();

    std::vector<epoll_event> loop(int timeout);
    bool addFd(int fd, int op);

private:
    static const int MaxEventSize = 100;
    int epollfd_;
    epoll_event evts_[MaxEventSize];
};