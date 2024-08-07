/*
socket 属性设置，以及其他相关操作封装。
主要是套接字本身的属性设置。
*/
#pragma once
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "InetAddress.h"

class Socket {
 public:
  // 创建一个非阻塞的socket
  static int createNonBlockSocket();

 public:
  Socket(int fd);
  ~Socket();

  // 快速重启：当一个应用程序关闭后，它占用的端口号可能会处于 TIME_WAIT 状态，
  // 而 TIME_WAIT 可以持续几分钟。
  // 在这个状态下，默认情况下，同一个端口号不能被再次使用。
  // 启用 SO_REUSEADDR 后，可以立即重启服务器监听同一个端口，而不需要等待
  // TIME_WAIT 状态结束。
  // 也允许多个套接字绑定到同一个端口
  void setReuseAddr(bool on);

  // 用于控制 TCP Nagle
  // 算法的启用或禁用。Nagle算法旨在减少小数据包在网络中的流量，它通过合并一些小的TCP包来减少网络上的负载。
  void setNoDelay(bool on);

  void bind(const InetAddress& addr);
  void listen(int backlog = 128);

  int accept(InetAddress& clientAddr);

  int fd() const;

 private:
  const int fd_;
};