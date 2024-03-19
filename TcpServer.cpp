#include "TcpServer.h"

#include <functional>

#include "Socket.h"
#include "Connector.h"

TcpServer::TcpServer(const std::string &ip, int port){
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setCreateConnectorCallback(std::bind(&TcpServer::createNewConnector, this,
                                                    std::placeholders::_1));
}

TcpServer::~TcpServer(){
    delete acceptor_;
}

void TcpServer::start(){
    loop_.run();
}

void TcpServer::createNewConnector(int clientFd){
    // TODO: 存在内存泄露需要优化
    Connector *conn = new Connector(&loop_, clientFd);
}