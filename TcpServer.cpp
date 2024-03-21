#include "TcpServer.h"

#include <cassert>
#include <functional>
#include <string.h>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port) {
  acceptor_ = new Acceptor(&loop_, ip, port);
  acceptor_->setCreateConnectorCallback(
      std::bind(&TcpServer::createNewConnector, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
  delete acceptor_;

  for (auto &&itr : connectors_) {
    delete itr.second;
  }
}

void TcpServer::start() { loop_.run(); }

void TcpServer::createNewConnector(int clientFd) {
  Connector *conn = new Connector(&loop_, clientFd);
  conn->setCloseCallback(
      std::bind(&TcpServer::connCloseCallback, this, std::placeholders::_1));
  conn->setErrorCallback(
      std::bind(&TcpServer::connErrorCallback, this, std::placeholders::_1));
  conn->setMessageCallback(std::bind(&TcpServer::onConnMessage, this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
  connectors_[conn->fd()] = conn;
}

void TcpServer::connCloseCallback(int fd) {
  printf("client(fd=%d) disconnect.\n", fd);

  assert(connectors_.end() != connectors_.find(fd));
  delete connectors_[fd];
  connectors_.erase(fd);
}

void TcpServer::connErrorCallback(int fd) {
  printf("client(fd=%d) error, close connection.\n", fd);

  assert(connectors_.end() != connectors_.find(fd));
  delete connectors_[fd];
  connectors_.erase(fd);
}

void TcpServer::onConnMessage(Connector *conn, const Buffer &msg) {
  std::string sendMsg;
  sendMsg.resize(4);
  uint32_t len = msg.size();
  memcpy(sendMsg.data(), (char *)&len, 4);
  sendMsg.append(msg.data(), msg.size());

  printf("recv(clientfd=%d):%s\n", conn->fd(), msg.data());
  conn->send(sendMsg.data(), sendMsg.length());
}