#include "Connector.h"

#include <functional>
#include <cassert>
#include <strings.h>
#include <assert.h>

Connector::Connector(EventLoop* loop, int clientfd){
    socket_ = new Socket(clientfd);

    chan_ = new Channel(clientfd, loop, false);
    chan_->setEvents(EPOLLIN | EPOLLET);
    chan_->setProcessInEvtFunc(std::bind(&Connector::onMessage, this));
    assert(loop->addChannel(chan_));
}

Connector::~Connector(){
    delete socket_;
    delete chan_;
}
    
bool Connector::onMessage(){
    // 边缘触发需要循环读取
    char buffer[1024];
    while (true){
        bzero(buffer, sizeof(buffer));

        int readLen = recv(fd(), buffer, sizeof(buffer), 0);
        if (readLen > 0){
            inBuf_.append(buffer, readLen);
        } else if (readLen == -1) {
            if (errno == EINTR)
            {
                // 读取时被信号中断，继续读取
                continue;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                outBuf_ = inBuf_;
                inBuf_.clear();

                printf("recv(clientfd=%d):%s\n", fd(), outBuf_.data());
                send(fd(), outBuf_.data(), outBuf_.size(), 0);
                // 全部数据读取完毕
                break;
            }
        } else if (readLen == 0) {
            // 客户端断开连接
            assert(connCloseCallback_ != nullptr);
            connCloseCallback_(fd());
            return true;
        }
    }

    return true;

}