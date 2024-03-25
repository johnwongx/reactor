/*回显服务器，主要用于添加业务相关代码*/
#pragma once
#include "TcpServer.h"

class EchoServer {
 public:
  EchoServer(const std::string& ip, int port);
  ~EchoServer();

  void Start() { tcpServ_.start(); }

  void HandleMessage(Connector* conn, const Buffer& msg);
  void HandleNewConnector(Connector* conn);
  void HandleClose(Connector* conn);

 private:
  TcpServer tcpServ_;
};