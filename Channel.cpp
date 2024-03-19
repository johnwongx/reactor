#include "Channel.h"

#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <cassert>

#include <sys/epoll.h>

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
        assert(connErrorCallback_ != nullptr);
        connCloseCallback_(fd_);
        return false;
    }

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
            assert(connCloseCallback_ != nullptr);
            connCloseCallback_(fd_);
            return true;
        }
    }

    return true;
}