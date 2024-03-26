#include "TcpServer.h"

#include <string.h>

#include <cassert>
#include <functional>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port, size_t threadNum)
    : threadNum_(threadNum) {
  mainLoop_ = std::make_shared<EventLoop>();
  mainLoop_->setEpollTimeoutCallback(
      std::bind(&TcpServer::onEpollTimeout, this, std::placeholders::_1));

  acceptor_ = std::make_shared<Acceptor>(mainLoop_, ip, port);
  acceptor_->setCreateConnectorCallback(
      std::bind(&TcpServer::createNewConnector, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {}

void TcpServer::start() { mainLoop_->run(); }

void TcpServer::createNewConnector(int clientFd) {
  ConnectorPtr conn = std::make_shared<Connector>(mainLoop_, clientFd);
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

  connectors_.erase(fd);
}

void TcpServer::connErrorCallback(int fd) {
  printf("client(fd=%d) error, close connection.\n", fd);

  assert(connectors_.end() != connectors_.find(fd));
  connectors_.erase(fd);
}

void TcpServer::onConnMessage(ConnectorPtr conn, const Buffer &msg) {
  if (messageCallback_) messageCallback_(conn, msg);
}

void TcpServer::onSendComplete(int fd) {
  printf("client(%d) send message success!\n", fd);
}

void TcpServer::onEpollTimeout(EventLoopPtr loop) {
  printf("Epoll timeout!\n");
}