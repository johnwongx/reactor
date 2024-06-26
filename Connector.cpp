#include "Connector.h"

#include <assert.h>
#include <strings.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cassert>
#include <functional>
#include <iostream>

Connector::Connector(EventLoop& loop, int clientfd, uint32_t maxIdleTime)
    : loop_(loop),
      disconnected_(false),
      socket_(std::make_unique<Socket>(clientfd)),
      maxIdleTime_(maxIdleTime) {
  auto now = std::chrono::system_clock::now();
  lastUpdateTime_ = std::chrono::time_point_cast<std::chrono::seconds>(now);

  chan_ = std::make_unique<Channel>(clientfd, loop);
  chan_->enableRead();
  chan_->enableET();
  chan_->setInEvtCallbackFunc(std::bind(&Connector::onMessage, this));
  chan_->setOutEvtCallbackFunc(std::bind(&Connector::onSend, this));
  chan_->setErrorCallback(
      std::bind(&Connector::OnError, this, std::placeholders::_1));
  chan_->flushEvents();
}

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
          lastUpdateTime_ = std::chrono::time_point_cast<std::chrono::seconds>(
              std::chrono::system_clock::now());

          inBuf_.erase(0, curBuf.size() + 4);
          messageCallback_(shared_from_this(), curBuf);
        }

        break;
      }
    } else if (readLen == 0) {
      // 客户端断开连接
      OnClose();
      return true;
    }
  }

  return true;
}

void Connector::Send(const Buffer& msg) {
  if (disconnected_) {
    return;
  }
  if (syscall(SYS_gettid) == loop_.GetThreadId()) {
    SendSync(msg);
  } else {
    // 加入事件循环的任务列表中
    loop_.PushTask(std::bind(&Connector::SendSync, this, msg));
  }
}

void Connector::SendSync(const Buffer& msg) {
  if (disconnected_) {
    return;
  }
  // 将数据加入到输出缓冲区，并监听写事件
  outBuf_.append(msg.data(), msg.size());
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
