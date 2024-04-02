#include "Connector.h"

#include <assert.h>
#include <strings.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cassert>
#include <functional>
#include <iostream>

Connector::Connector(EventLoopPtr loop, int clientfd) : disconnected_(false) {
  socket_ = std::make_shared<Socket>(clientfd);

  chan_ = std::make_shared<Channel>(clientfd, loop, false);
  chan_->enableRead();
  chan_->enableET();
  chan_->setInEvtCallbackFunc(std::bind(&Connector::onMessage, this));
  chan_->setOutEvtCallbackFunc(std::bind(&Connector::onSend, this));
  chan_->setCloseCallback(
      std::bind(&Connector::OnClose, this, std::placeholders::_1));
  chan_->setErrorCallback(
      std::bind(&Connector::OnError, this, std::placeholders::_1));
  loop->updateChannel(chan_);
}

Connector::~Connector() { std::cout << "Connector::~Connector()" << std::endl; }

bool Connector::onMessage() {
  // printf("Connector::onMessage() thread(%ld).\n", syscall(SYS_gettid));

  // 边缘触发需要循环读取
  char buffer[1024];
  while (true) {
    bzero(buffer, sizeof(buffer));

    int readLen = recv(fd(), buffer, sizeof(buffer), 0);
    if (readLen > 0) {
      inBuf_.append(buffer, readLen);
    } else if (readLen == -1) {
      if (errno == EINTR) {
        // 读取时被信号中断，继续读取
        continue;
      } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
        Buffer curBuf;
        while (true) {
          if (!curBuf.readOneMessageWith32Header(inBuf_)) {
            // 读取消息不完整，等待完整后再次读取
            break;
          }

          inBuf_.erase(0, curBuf.size() + 4);
          messageCallback_(shared_from_this(), curBuf);
        }

        break;
      }
    } else if (readLen == 0) {
      // 客户端断开连接
      OnClose(fd());
      return true;
    }
  }

  return true;
}

void Connector::send(const Buffer& info) {
  if (disconnected_) {
    std::cout << "连接已断开，发送直接返回" << std::endl;
    return;
  }
  // 将数据加入到输出缓冲区，并监听写事件
  outBuf_.append(info.data(), info.size());
  chan_->enableWrite();
  chan_->flushEvents();
}

bool Connector::onSend() {
  if (disconnected_) return false;
  // 发送缓冲区中的事件
  if (outBuf_.size() > 0) {
    int sendLen = ::send(fd(), outBuf_.data(), outBuf_.size(), 0);
    if (sendLen < 0) {
      perror("send() failed");
      return false;
    }
    outBuf_.erase(0, sendLen);
  }

  // 所有数据发送完毕后取消读事件监听
  if (outBuf_.size() == 0) {
    chan_->disableWrite();
    chan_->flushEvents();

    sendCompleteCallback_(fd());
  }
  return true;
}