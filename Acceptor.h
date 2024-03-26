/*
listen channel封装
*/
#pragma once
#include <memory>
#include <string>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class Acceptor {
 public:
  Acceptor(EventLoopPtr loop, const std::string& ip, int port);
  ~Acceptor();

  // 有新的客户端连接
  bool onNewConnection();

  void setCreateConnectorCallback(std::function<void(int)> fn) {
    connectorCallback_ = fn;
  }

 private:
  SocketPtr socket_;
  ChannelPtr chan_;
  std::function<void(int)> connectorCallback_;
};

typedef std::shared_ptr<Acceptor> AcceptorPtr;