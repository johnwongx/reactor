#include "TcpServer.h"

#include <string.h>

#include <cassert>
#include <functional>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port) {
  loop_.setEpollTimeoutCallback(
      std::bind(&TcpServer::onEpollTimeout, this, std::placeholders::_1));

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
  conn->setSendCompleteCallback(
      std::bind(&TcpServer::onSendComplete, this, std::placeholders::_1));
  connectors_[conn->fd()] = conn;

  if (newConnectorCallback_) newConnectorCallback_(conn);
}

void TcpServer::connCloseCallback(int fd) {
  assert(connectors_.end() != connectors_.find(fd));

  if (connectorCloseCallback_) connectorCloseCallback_(connectors_[fd]);

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
  if (messageCallback_) messageCallback_(conn, msg);
}

void TcpServer::onSendComplete(int fd) {
  printf("client(%d) send message success!\n", fd);
}

void TcpServer::onEpollTimeout(EventLoop *loop) { printf("Epoll timeout!\n"); }