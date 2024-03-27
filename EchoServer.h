/*回显服务器，主要用于添加业务相关代码*/
#pragma once
#include "TcpServer.h"

class EchoServer {
 public:
  EchoServer(const std::string& ip, int port, size_t subThreadNum = 3,
             size_t workThreadNum = 5);
  ~EchoServer();

  void Start() { tcpServ_.start(); }

  void HandleMessage(ConnectorPtr conn, const Buffer& msg);
  void HandleNewConnector(ConnectorPtr conn);
  void HandleClose(ConnectorPtr conn);

  void OnMessage(ConnectorPtr conn, const Buffer& msg);

 private:
  TcpServer tcpServ_;
  ThreadPoolPtr workThreadPool_;
};