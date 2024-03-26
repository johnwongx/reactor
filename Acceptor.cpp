#include "Acceptor.h"

#include <cassert>
#include <functional>

Acceptor::Acceptor(EventLoopPtr loop, const std::string &ip,
                   int port) /*:loop_(loop)*/ {
  int listenfd = Socket::createNonBlockSocket();

  socket_ = std::make_shared<Socket>(listenfd);
  socket_->setReuseAddr(true);
  socket_->setNoDelay(true);

  InetAddress servAddr("127.0.0.1", port);
  socket_->bind(servAddr);

  socket_->listen();

  chan_ = std::make_shared<Channel>(listenfd, loop, true);
  chan_->enableRead();
  chan_->setInEvtCallbackFunc(std::bind(&Acceptor::onNewConnection, this));
  loop->updateChannel(chan_);
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
