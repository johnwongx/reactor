#include "Connector.h"

#include <functional>
#include <cassert>

Connector::Connector(EventLoop* loop, int clientfd){
    socket_ = new Socket(clientfd);

    chan_ = new Channel(clientfd, loop, false);
    chan_->setEvents(EPOLLIN | EPOLLET);
    chan_->setProcessInEvtFunc(std::bind(&Channel::onMessage, chan_));
    assert(loop->addChannel(chan_));
}

Connector::~Connector(){
    delete socket_;
    delete chan_;
}