#include "EchoServer.h"

#include <string.h>

EchoServer::EchoServer(const std::string& ip, int port, size_t threadNum)
    : tcpServ_(ip, port, threadNum) {
  tcpServ_.setNewConnectorCallback(
      std::bind(&EchoServer::HandleNewConnector, this, std::placeholders::_1));
  tcpServ_.setConnectorCloseCallback(
      std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
  tcpServ_.setMessageCallback(std::bind(&EchoServer::HandleMessage, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
}

EchoServer::~EchoServer() {}

void EchoServer::HandleMessage(ConnectorPtr conn, const Buffer& msg) {
  std::string sendMsg("recv:");
  sendMsg.append(msg.data(), msg.size());
  Buffer sendBuf;
  sendBuf.appendWithHeader(sendMsg.data(), sendMsg.size());

  printf("recv(clientfd=%d):%s\n", conn->fd(), msg.data());
  conn->send(sendBuf);
}

void EchoServer::HandleNewConnector(ConnectorPtr conn) {
  printf("accept new connector(%d).\n", conn->fd());
}

void EchoServer::HandleClose(ConnectorPtr conn) {
  printf("connector(%d) disconnec.\n", conn->fd());
}