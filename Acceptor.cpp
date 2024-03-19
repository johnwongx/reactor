#include "Acceptor.h"

#include <functional>

Acceptor::Acceptor(EventLoop* loop, const std::string &ip, int port)/*:loop_(loop)*/{
    int listenfd = Socket::createNonBlockSocket();

    socket_ = new Socket(listenfd);
    socket_ ->setReuseAddr(true);
    socket_->setNoDelay(true);

    InetAddress servAddr("127.0.0.1", port);
    socket_->bind(servAddr);

    socket_->listen();

    chan_ = new Channel(listenfd, loop, true);
    chan_->appendEvent(EPOLLIN);
    chan_->setProcessInEvtFunc(std::bind(&Channel::onNewConnection, chan_, socket_));
    loop->addChannel(chan_);
}

Acceptor::~Acceptor(){
    delete socket_;
    delete chan_;
}