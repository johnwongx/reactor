/*
服务器封装，负责启动与运行
*/
#pragma once
#include <map>
#include <memory>
#include <string>

#include "Acceptor.h"
#include "Buffer.h"
#include "Connector.h"
#include "EventLoop.h"
#include "utils/ThreadPool.h"

class TcpServer {
 public:
  TcpServer(const std::string &ip, int port, size_t threadNum);
  ~TcpServer();

  void start();

  void createNewConnector(int clientFd);

  void connCloseCallback(int fd);
  void connErrorCallback(int fd);

  void onConnMessage(ConnectorPtr conn, const Buffer &msg);

  void onSendComplete(int fd);

  void onEpollTimeout(EventLoopPtr loop);

  void setMessageCallback(
      std::function<void(ConnectorPtr, const Buffer &msg)> fn) {
    messageCallback_ = fn;
  }

  void setNewConnectorCallback(std::function<void(ConnectorPtr)> fn) {
    newConnectorCallback_ = fn;
  }

  void setConnectorCloseCallback(std::function<void(ConnectorPtr)> fn) {
    connectorCloseCallback_ = fn;
  }

 private:
  EventLoopPtr mainLoop_;
  std::vector<EventLoopPtr> subLoops_;
  ThreadPoolPtr threadPool_;
  size_t threadNum_;
  AcceptorPtr acceptor_;
  std::map<int /*fd*/, ConnectorPtr> connectors_;

  std::function<void(ConnectorPtr, const Buffer &msg)> messageCallback_;
  std::function<void(ConnectorPtr)> newConnectorCallback_;
  std::function<void(ConnectorPtr)> connectorCloseCallback_;
};

typedef std::shared_ptr<TcpServer> TcpServerPtr;