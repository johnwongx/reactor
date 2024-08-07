/*
业务处理层，增加了线程池用于快速处理业务，防止阻塞底层网络。
回显服务器，主要用于添加业务相关代码。
*/
#pragma once
#include "TcpServer.h"

class EchoServer {
 public:
  EchoServer(const std::string& ip, int port, size_t subThreadNum = 3,
             size_t workThreadNum = 5);
  ~EchoServer();

  void Start() { tcpServ_.Start(); }
  void Stop();

 private:
  void HandleMessage(std::weak_ptr<Connector> conn, const Buffer& msg);
  void OnMessage(std::weak_ptr<Connector> conn, const Buffer& msg);

  void HandleNewConnector(Connector& conn);
  void HandleClose(Connector& conn);

 private:
  TcpServer tcpServ_;
  std::unique_ptr<ThreadPool> workThreadPool_;
};