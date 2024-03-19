#include "TcpServer.h"

#include <functional>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port){
    int listenfd = Socket::createNonBlockSocket();
    // TODO: delete socket
    Socket *servSoc = new Socket(listenfd);
    servSoc->setReuseAddr(true);
    servSoc->setNoDelay(true);

    InetAddress servAddr("127.0.0.1", port);
    servSoc->bind(servAddr);

    servSoc->listen();

    // TODO: 释放资源
    Channel* servChan = new Channel(listenfd, &loop_, true);
    servChan->appendEvent(EPOLLIN);
    servChan->setProcessInEvtFunc(std::bind(&Channel::onNewConnection, servChan, servSoc));
    loop_.addChannel(servChan);
}

TcpServer::~TcpServer(){
}

void TcpServer::start(){
    loop_.run();
}