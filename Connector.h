/*
客户端连接channel管理
*/
#pragma once
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

class Connector {
public:
    Connector(EventLoop* loop, int clientfd);
    ~Connector();

private:
    Socket* socket_;
    Channel* chan_;
};