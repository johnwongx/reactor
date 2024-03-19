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

private:
   // EventLoop* loop_; 
   Socket* socket_;
   Channel* chan_;
};