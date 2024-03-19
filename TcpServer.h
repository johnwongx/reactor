/*
服务器封装，负责启动与运行
*/
#pragma once
#include <string>

#include "EventLoop.h"
#include "Acceptor.h"

class TcpServer
{
public:
    TcpServer(const std::string& ip, int port);
    ~TcpServer();

    void start();

    void createNewConnector(int clientFd);

private:
   EventLoop loop_; 
   Acceptor* acceptor_;
};
