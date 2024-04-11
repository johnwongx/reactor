/*
客户端连接channel管理
*/
#pragma once
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  Connector(EventLoop &loop, int clientfd,
            uint32_t maxIdleTime = 10 /*seconds*/);
  ~Connector() = default;

  int fd() const { return socket_->fd(); }

  void setCloseCallback(std::function<void(int)> fn) {
    connCloseCallback_ = fn;
  }

  void setErrorCallback(std::function<void(int)> fn) {
    connErrorCallback_ = fn;
  }

  void OnClose() {
    disconnected_ = true;
    chan_->Remove();
    if (connCloseCallback_) connCloseCallback_(chan_->fd());
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

  // 是否超过最大空闲时间
  bool IsTimeout() const {
    auto now = std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
    auto diff = now - lastUpdateTime_;
    return diff > maxIdleTime_;
  }

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

  std::chrono::seconds maxIdleTime_;
  std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
      lastUpdateTime_;
};

typedef std::shared_ptr<Connector> ConnectorPtr;