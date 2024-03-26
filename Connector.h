/*
客户端连接channel管理
*/
#pragma once
#include <memory>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  Connector(EventLoopPtr loop, int clientfd);
  ~Connector();

  int fd() const { return socket_->fd(); }

  void setCloseCallback(std::function<void(int)> fn) {
    connCloseCallback_ = fn;
    chan_->setCloseCallback(fn);
  }

  void setErrorCallback(std::function<void(int)> fn) {
    chan_->setErrorCallback(fn);
  }

  void setMessageCallback(
      std::function<void(std::shared_ptr<Connector>, const Buffer &)> fn) {
    messageCallback_ = fn;
  }

  void setSendCompleteCallback(std::function<void(int)> fn) {
    sendCompleteCallback_ = fn;
  }

  void send(const Buffer &info);

 private:
  bool onMessage();
  bool onSend();

 private:
  SocketPtr socket_;
  ChannelPtr chan_;

  Buffer inBuf_;
  Buffer outBuf_;

  std::function<void(int)> connCloseCallback_;
  std::function<void(std::shared_ptr<Connector>, const Buffer &)>
      messageCallback_;
  std::function<void(int)> sendCompleteCallback_;
};

typedef std::shared_ptr<Connector> ConnectorPtr;