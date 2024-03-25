/*
服务器封装，负责启动与运行
*/
#pragma once
#include <map>
#include <string>

#include "Acceptor.h"
#include "Buffer.h"
#include "Connector.h"
#include "EventLoop.h"

class TcpServer {
 public:
  TcpServer(const std::string &ip, int port);
  ~TcpServer();

  void start();

  void createNewConnector(int clientFd);

  void connCloseCallback(int fd);
  void connErrorCallback(int fd);

  void onConnMessage(Connector *conn, const Buffer &msg);

  void onSendComplete(int fd);

  void onEpollTimeout(EventLoop *loop);

  void setMessageCallback(
      std::function<void(Connector *, const Buffer &msg)> fn) {
    messageCallback_ = fn;
  }

  void setNewConnectorCallback(std::function<void(Connector *)> fn) {
    newConnectorCallback_ = fn;
  }

  void setConnectorCloseCallback(std::function<void(Connector *)> fn) {
    connectorCloseCallback_ = fn;
  }

 private:
  EventLoop loop_;
  Acceptor *acceptor_;
  std::map<int /*fd*/, Connector *> connectors_;

  std::function<void(Connector *, const Buffer &msg)> messageCallback_;
  std::function<void(Connector *)> newConnectorCallback_;
  std::function<void(Connector *)> connectorCloseCallback_;
};
