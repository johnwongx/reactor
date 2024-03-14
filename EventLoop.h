#pragma once
#include "Epoll.h"
#include "Channel.h"

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void run();

    bool addChannel(Channel* chan);

private:
    Epoll* ep_;
};