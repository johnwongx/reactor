#include "Acceptor.h"

#include <cassert>
#include <functional>

Acceptor::Acceptor(EventLoop& loop, const std::string& ip, int port) {
  int listenfd = Socket::createNonBlockSocket();

  socket_ = std::make_unique<Socket>(listenfd);
  socket_->setReuseAddr(true);
  socket_->setNoDelay(true);

  InetAddress servAddr("127.0.0.1", port);
  socket_->bind(servAddr);

  socket_->listen();

  chan_ = std::make_unique<Channel>(listenfd, loop);
  chan_->enableRead();
  chan_->setInEvtCallbackFunc(std::bind(&Acceptor::onNewConnection, this));
  loop.updateChannel(*chan_);
}

Acceptor::~Acceptor() {}

bool Acceptor::onNewConnection() {
  InetAddress clientAddr;
  int clientfd = socket_->accept(clientAddr);
  if (clientfd < 0) {
    perror("accept()");
    return false;
  }
  printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd,
         clientAddr.ip(), clientAddr.port());

  assert(connectorCallback_ != nullptr);
  connectorCallback_(clientfd);

  return true;
}
