#include "TcpServer.h"

#include <string.h>

#include <cassert>
#include <functional>

#include "Socket.h"

TcpServer::TcpServer(const std::string &ip, int port, size_t threadNum)
    : threadNum_(threadNum) {
  mainLoop_ = std::make_unique<EventLoop>(-1);
  mainLoop_->setEpollTimeoutCallback(
      std::bind(&TcpServer::OnEpollTimeout, this, std::placeholders::_1));

  threadPool_ = std::make_unique<ThreadPool>(threadNum_, "Eventloop");
  for (size_t i = 0; i < threadNum_; i++) {
    std::unique_ptr<EventLoop> loop = std::make_unique<EventLoop>();
    loop->setEpollTimeoutCallback(
        std::bind(&TcpServer::OnEpollTimeout, this, std::placeholders::_1));
    subLoops_.emplace_back(std::move(loop));

    threadPool_->AddTask([&, i] { subLoops_[i]->Run(); });
  }

  acceptor_ = std::make_unique<Acceptor>(*mainLoop_, ip, port);
  acceptor_->setCreateConnectorCallback(
      std::bind(&TcpServer::CreateNewConnector, this, std::placeholders::_1));
}

TcpServer::~TcpServer() { Stop(); }

void TcpServer::Start() { mainLoop_->Run(); }

void TcpServer::Stop() {
  mainLoop_->Stop();
  for (size_t i = 0; i < subLoops_.size(); i++) {
    subLoops_[i]->Stop();
  }
  threadPool_->Stop();
}

void TcpServer::CreateNewConnector(int clientFd) {
  std::unique_ptr<EventLoop> &loop = subLoops_[clientFd % threadNum_];

  ConnectorPtr conn = std::make_shared<Connector>(*loop, clientFd);
  conn->setCloseCallback(
      std::bind(&TcpServer::ConnCloseCallback, this, std::placeholders::_1));
  conn->setErrorCallback(
      std::bind(&TcpServer::ConnErrorCallback, this, std::placeholders::_1));
  conn->setMessageCallback(std::bind(&TcpServer::OnConnMessage, this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
  conn->setSendCompleteCallback(
      std::bind(&TcpServer::OnSendComplete, this, std::placeholders::_1));
  connectors_[conn->fd()] = conn;
  loop->AddConnector(conn);

  if (newConnectorCallback_) newConnectorCallback_(*conn);
}

void TcpServer::ConnCloseCallback(int fd) {
  printf("TcpServer::connCloseCallback(fd=%d).\n", fd);

  assert(connectors_.end() != connectors_.find(fd));
  if (connectorCloseCallback_) connectorCloseCallback_(*connectors_[fd]);
  connectors_.erase(fd);
}

void TcpServer::ConnErrorCallback(int fd) {
  printf("TcpServer::connErrorCallback(fd=%d).\n", fd);

  assert(connectors_.end() != connectors_.find(fd));
  connectors_.erase(fd);
}

void TcpServer::OnConnMessage(ConnectorPtr conn, const Buffer &msg) {
  if (messageCallback_) messageCallback_(conn, msg);
}