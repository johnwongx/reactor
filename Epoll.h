#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll
{
public:
    Epoll();
    ~Epoll();

    std::vector<Channel*> loop(int timeout);
    bool addChannel(Channel* chan);

private:
    int epollfd_;

    static const int MaxEventSize = 100;
    epoll_event evts_[MaxEventSize];
};