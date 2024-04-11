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

  void Start();
  void Stop();

  void CreateNewConnector(int clientFd);

  void ConnCloseCallback(int fd);
  void ConnErrorCallback(int fd);

  void OnConnMessage(ConnectorPtr conn, const Buffer &msg);

  void OnSendComplete(int fd) {}

  void OnEpollTimeout(EventLoop &loop) {}

  void SetMessageCallback(
      std::function<void(std::weak_ptr<Connector>, const Buffer &msg)> fn) {
    messageCallback_ = fn;
  }

  void SetNewConnectorCallback(std::function<void(Connector &)> fn) {
    newConnectorCallback_ = fn;
  }

  void SetConnectorCloseCallback(std::function<void(Connector &)> fn) {
    connectorCloseCallback_ = fn;
  }

 private:
  std::unique_ptr<EventLoop> mainLoop_;
  std::unique_ptr<Acceptor> acceptor_;

  size_t threadNum_;
  std::vector<std::unique_ptr<EventLoop>> subLoops_;
  std::unique_ptr<ThreadPool> threadPool_;
  // 本类管理connector的生命周期，为方便外部在多线程环境下使用时方便判断connector是否有效
  // 使用shared_ptr, 而非unique_ptr.
  // 向外传出时只传出weak_ptr或者Connector&, 以此保证对于Connector的生命控制
  std::map<int /*fd*/, ConnectorPtr> connectors_;

  std::function<void(std::weak_ptr<Connector>, const Buffer &msg)>
      messageCallback_;
  std::function<void(Connector &)> newConnectorCallback_;
  std::function<void(Connector &)> connectorCloseCallback_;
};

typedef std::shared_ptr<TcpServer> TcpServerPtr;