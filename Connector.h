/*
客户端连接channel管理
*/
#pragma once
#include <atomic>
#include <memory>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  Connector(EventLoop &loop, int clientfd);
  ~Connector();

  int fd() const { return socket_->fd(); }

  void setCloseCallback(std::function<void(int)> fn) {
    connCloseCallback_ = fn;
  }

  void setErrorCallback(std::function<void(int)> fn) {
    connErrorCallback_ = fn;
  }

  void OnClose(int fd) {
    disconnected_ = true;
    chan_->Remove();
    if (connCloseCallback_) connCloseCallback_(fd);
  }

  void OnError(int fd) {
    disconnected_ = true;
    chan_->Remove();
    if (connErrorCallback_) connErrorCallback_(fd);
  }

  void setMessageCallback(
      std::function<void(std::shared_ptr<Connector>, const Buffer &)> fn) {
    messageCallback_ = fn;
  }

  void setSendCompleteCallback(std::function<void(int)> fn) {
    sendCompleteCallback_ = fn;
  }

  // 如果和事件循环在同一个类中将直接发送，否则异步发送
  void Send(const Buffer &info);

 private:
  bool onMessage();
  bool onSend();
  // 直接发送
  void SendSync(const Buffer &msg);

 private:
  EventLoop &loop_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> chan_;
  std::atomic_bool disconnected_;

  Buffer inBuf_;
  Buffer outBuf_;

  std::function<void(int)> connCloseCallback_;
  std::function<void(int)> connErrorCallback_;
  std::function<void(std::shared_ptr<Connector>, const Buffer &)>
      messageCallback_;
  std::function<void(int)> sendCompleteCallback_;
};

typedef std::shared_ptr<Connector> ConnectorPtr;