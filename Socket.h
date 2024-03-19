/*
socket属性设置，以及其他相关操作封装
*/
#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string>
#include <errno.h>
#include <unistd.h>

#include "InetAddress.h"

class Socket
{
public:
    // 创建一个非阻塞的socket
    static int createNonBlockSocket();

public:
    Socket(int fd);
    ~Socket();

    void setReuseAddr(bool on);
    void setNoDelay(bool on);

    void bind(const InetAddress& addr);
    void listen(int backlog = 128);

    int accept(InetAddress& clientAddr);

    int fd() const;

private:
    const int fd_;
};