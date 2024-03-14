#include "Channel.h"

#include <strings.h>

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
    // TODO: 存在内存泄露需要优化
    Socket *clientSoc = new Socket(clientfd);

    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientSoc->fd(),
           clientAddr.ip(), clientAddr.port());

    Channel *clientChan = new Channel(clientfd, epoll_, false);
    clientChan->setEvents(EPOLLIN | EPOLLET);
    clientChan->setProcessInEvtFunc(std::bind(&Channel::onMessage, clientChan));
    if (!epoll_->addChannel(clientChan))
        return false;
    
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