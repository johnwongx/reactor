/*
服务器封装，负责启动与运行
*/
#pragma once
#include <string>
#include <map>

#include "EventLoop.h"
#include "Acceptor.h"
#include "Connector.h"

class TcpServer
{
public:
    TcpServer(const std::string& ip, int port);
    ~TcpServer();

    void start();

    void createNewConnector(int clientFd);

    void connCloseCallback(int fd);
    void connErrorCallback(int fd);

private:
   EventLoop loop_; 
   Acceptor* acceptor_;
   std::map<int /*fd*/, Connector*> connectors_;
};
