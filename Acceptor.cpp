#include "Acceptor.h"

#include <functional>
#include <cassert>

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
    chan_->setProcessInEvtFunc(std::bind(&Acceptor::onNewConnection, this));
    loop->addChannel(chan_);
}

Acceptor::~Acceptor(){
    delete socket_;
    delete chan_;
}

bool Acceptor::onNewConnection()
{ 
    InetAddress clientAddr;
    int clientfd = socket_->accept(clientAddr);
    if (clientfd < 0)
    {
        perror("accept()");
        return false;
    }
    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd,
           clientAddr.ip(), clientAddr.port());

    assert(connectorCallback_ != nullptr);
    connectorCallback_(clientfd);

    return true;
}
