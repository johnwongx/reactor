#include "TcpServer.h"

#include <functional>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port){
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer(){
    delete acceptor_;
}

void TcpServer::start(){
    loop_.run();
}