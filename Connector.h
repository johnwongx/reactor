/*
客户端连接channel管理
*/
#pragma once
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connector {
public:
    Connector(EventLoop* loop, int clientfd);
    ~Connector();

    int fd() const {
        return socket_->fd();
    }

    void setCloseCallback(std::function<void(int)> fn) {
        connCloseCallback_ = fn;
        chan_->setCloseCallback(fn);
    }

    void setErrorCallback(std::function<void(int)> fn) {
        chan_->setErrorCallback(fn);
    }

private:
    bool onMessage();

private:
    Socket* socket_;
    Channel* chan_;

    Buffer inBuf_;
    Buffer outBuf_;
    
    std::function<void(int)> connCloseCallback_;
};