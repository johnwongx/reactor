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
