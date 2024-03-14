#include "EventLoop.h"

#include <iostream>

EventLoop::EventLoop():ep_(new Epoll()){
}

EventLoop::~EventLoop(){
    delete ep_;
    ep_ = nullptr;
}

bool EventLoop::addChannel(Channel* chan){
    return ep_->addChannel(chan);
}

void EventLoop::run(){
    while (true){
        std::vector<Channel *> chanList = ep_->loop(-1);
        if (chanList.empty()){
            std::cout << "wait timeout" << std::endl;
            continue;
        }

        for (const auto &chanItr : chanList)
        {
            if (!chanItr->handleEvent())
            {
                std::cout << "handle event failed!" << std::endl;
                continue;
            }
        }
    }
}