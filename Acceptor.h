/*
处理新连接，收到新连接的事件后, 接收连接，然后通知外部处理。

listen channel 封装
*/
#pragma once
#include <memory>
#include <string>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class Acceptor {
 public:
  Acceptor(EventLoop& loop, const std::string& ip, int port);
  ~Acceptor();

  // 有新的客户端连接
  bool onNewConnection();

  void setCreateConnectorCallback(std::function<void(int)> fn) {
    connectorCallback_ = fn;
  }

 private:
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> chan_;
  std::function<void(int)> connectorCallback_;
};