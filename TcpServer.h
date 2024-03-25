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

 private:
  EventLoop loop_;
  Acceptor *acceptor_;
  std::map<int /*fd*/, Connector *> connectors_;
};
