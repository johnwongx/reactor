/*
listen channel封装
*/
#pragma once
#include <string>

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class Acceptor{
public:
    Acceptor(EventLoop* loop, const std::string &ip, int port);
    ~Acceptor();

    // 有新的客户端连接
    bool onNewConnection();

    void setCreateConnectorCallback(std::function<void(int)> fn){
        connectorCallback_ = fn;
    }

private:
   // EventLoop* loop_; 
   Socket* socket_;
   Channel* chan_;
   std::function<void(int)> connectorCallback_;
};