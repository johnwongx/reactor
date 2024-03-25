#include "EchoServer.h"

#include <string.h>

EchoServer::EchoServer(const std::string& ip, int port) : tcpServ_(ip, port) {
  tcpServ_.setNewConnectorCallback(
      std::bind(&EchoServer::HandleNewConnector, this, std::placeholders::_1));
  tcpServ_.setConnectorCloseCallback(
      std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
  tcpServ_.setMessageCallback(std::bind(&EchoServer::HandleMessage, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
}

EchoServer::~EchoServer() {}

void EchoServer::HandleMessage(Connector* conn, const Buffer& msg) {
  std::string sendMsg;
  sendMsg.resize(4);
  uint32_t len = msg.size();
  memcpy(sendMsg.data(), (char*)&len, 4);
  sendMsg.append(msg.data(), msg.size());

  printf("recv(clientfd=%d):%s\n", conn->fd(), msg.data());
  conn->send(sendMsg.data(), sendMsg.length());
}

void EchoServer::HandleNewConnector(Connector* conn) {
  printf("accept new connector(%d).\n", conn->fd());
}

void EchoServer::HandleClose(Connector* conn) {
  printf("connector(%d) disconnec.\n", conn->fd());
}