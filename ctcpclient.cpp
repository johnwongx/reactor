#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "Buffer.h"

using namespace std;

class ctcpclient {
 public:
  ctcpclient() : m_sockfd(-1) {}
  ~ctcpclient() { close(); }

  bool connect(const std::string &ip, const unsigned short port) {
    if (m_sockfd != -1) return false;

    // 第1步：创建客户端的socket。
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) return false;
    printf("client fd:%d\n", m_sockfd);

    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    // setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
    // sizeof(tv));

    // 第2步：向服务器发起连接请求。
    struct sockaddr_in servaddr;  // 用于存放协议、端口和IP地址的结构体。
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;    // ①协议族，固定填AF_INET。
    servaddr.sin_port = htons(port);  // ②指定服务端的通信端口。

    struct hostent *h = gethostbyname(
        ip.data());  // 用于存放服务端IP地址(大端序)的结构体的指针。
    if (!h)  // 把域名/主机名/字符串格式的IP转换成结构体。
    {
      close();
      return false;
    }
    memcpy(&servaddr.sin_addr, h->h_addr,
           h->h_length);  // ③指定服务端的IP(大端序)。

    if (::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) ==
        -1)  // 向服务端发起连接清求。
    {
      close();
      return false;
    }

    return true;
  }

  bool send(const string &info) {
    if (m_sockfd == -1) return false;

    int iret = ::send(m_sockfd, info.data(), info.size(), 0);
    if (iret <= 0) return false;
    return true;
  }

  bool send(const void *buffer, int size) {
    if (m_sockfd == -1) return false;

    int iret = ::send(m_sockfd, buffer, size, 0);
    if (iret <= 0) return false;

    return true;
  }

  bool recv(string &buffer, int maxLen) {
    if (m_sockfd == -1) return false;

    buffer.clear();
    buffer.resize(maxLen);
    int readn = ::recv(m_sockfd, &buffer[0], buffer.size(), 0);
    if (readn <= 0) {
      buffer.clear();
      return false;
    }
    buffer.resize(readn);

    return true;
  }

  bool close() {
    if (m_sockfd == -1) return false;

    ::close(m_sockfd);
    m_sockfd = -1;
    return true;
  }

 public:
  int m_sockfd;
  std::string m_ip;
  unsigned short m_port;
};

int main(int argc, char *argv[]) {
  string ip = "localhost";
  const unsigned short port = 5005;

  ctcpclient c;
  if (!c.connect(ip, port)) {
    perror("connect() failed!");
    return -1;
  }

  char buffer[1024];
  size_t kMsgCount = 2;
  const uint32_t kHeaderLen = sizeof(uint32_t);
  for (int i = 0; i < kMsgCount; i++) {
    uint32_t len = 0;
    memset(buffer, 0, kHeaderLen);
    sprintf(buffer + kHeaderLen, "这是第%d条信息。", i + 1);
    len = strlen(buffer + kHeaderLen);

    memcpy(buffer, (char *)&len, kHeaderLen);
    if (!c.send(buffer, len + kHeaderLen)) {
      perror("send()");
      break;
    }
    sleep(10);
  }

  int readCnt = 0;
  Buffer inBuf;
  while (readCnt < kMsgCount) {
    memset(buffer, 0, sizeof(buffer));
    int recvLen = recv(c.m_sockfd, buffer, sizeof(buffer), 0);
    if (recvLen <= 0) {
      perror("recv()");
      break;
    }
    inBuf.append(buffer, recvLen);

    while (inBuf.size() > 0) {
      Buffer oneMsg;
      if (!oneMsg.readOneMessageWith32Header(inBuf)) {
        // 读取不完整，留着下次一起读取
        break;
      } else {
        printf("recv(%d):%s\n", readCnt, oneMsg.data());
        inBuf.erase(0, kHeaderLen + oneMsg.size());
        readCnt++;
      }
    }
  }

  return 0;
}