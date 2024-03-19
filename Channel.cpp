#include "Channel.h"

#include <strings.h>
#include <stdio.h>

#include <sys/epoll.h>
#include <unistd.h>

#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Connector.h"

bool Channel::handleEvent()
{
    if (rEvents_ & EPOLLIN)
    {
        if(!inEvtFunc_)
            return false;
        
        return inEvtFunc_();
    }
    else if (rEvents_ & EPOLLOUT)
    {
        // 写事件处理
    }
    else
    {
        // 其他事件都视为错误
        printf("client(fd=%d) error.\n", fd_);
        close(fd_);
        return false;
    }

    return true;
}

bool Channel::onNewConnection(Socket *servSock)
{ // 有新的客户端连接
    InetAddress clientAddr;
    int clientfd = servSock->accept(clientAddr);
    if (clientfd < 0)
    {
        perror("accept()");
        return false;
    }
    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd,
           clientAddr.ip(), clientAddr.port());

    // TODO: 存在内存泄露需要优化
    Connector *conn = new Connector(loop_, clientfd);

    return true;
}

bool Channel::onMessage()
{ // 边缘触发需要循环读取
    char buffer[1024];
    while (true)
    {
        bzero(buffer, sizeof(buffer));

        int readLen = recv(fd_, buffer, sizeof(buffer), 0);
        if (readLen > 0)
        {
            printf("recv(clientfd=%d):%s\n", fd_, buffer);
            send(fd_, buffer, readLen, 0);
        }
        else if (readLen == -1)
        {
            if (errno == EINTR)
            {
                // 读取时被信号中断，继续读取
                continue;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 全部数据读取完毕
                break;
            }
        }
        else if (readLen == 0)
        {
            // 客户端断开连接
            printf("client(fd=%d) disconnect.\n", fd_);
            close(fd_);
            return true;
        }
    }

    return true;
}