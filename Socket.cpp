#include "Socket.h"

#include <fcntl.h>

int Socket::createNonBlockSocket()
{
    int listenfd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); 
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd)
{
}

Socket::~Socket()
{
    ::close(fd_);
}

void Socket::setReuseAddr(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::setNoDelay(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::bind(const InetAddress &addr)
{
    int ret = ::bind(fd_, addr.addr(), sizeof(sockaddr));
    if (ret != 0)
    {
        perror("bind() failed!");
        exit(-1);
    }
    
}
void Socket::listen(int backlog)
{
    int ret = ::listen(fd_, backlog);
    if (ret != 0)
    {
        perror("listen() failed!");
        exit(-1);
    }
}

int Socket::accept(InetAddress &clientAddr)
{
    sockaddr_in peerAddr;
    socklen_t sockLen = sizeof peerAddr;
    int clientfd = accept4(fd_, (sockaddr *)&peerAddr, &sockLen, O_NONBLOCK);

    clientAddr.setAddr(peerAddr);
    return clientfd;
}

int Socket::fd() const
{
    return fd_;
}