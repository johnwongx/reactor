#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "InetAddress.h"

using namespace std;

int main(int argc, char *argv[])
{
    const unsigned short port = 5005;

    int listenfd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, IPPROTO_TCP);
    if(listenfd < 0)
    {
        perror("socket()");
        return -1;
    }

    // 设置listenfd属性
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));

    InetAddress servAddr("127.0.0.1", port);
    if(bind(listenfd, servAddr.addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed!");
        close(listenfd);
        return -1;
    }

    if(listen(listenfd, 128) != 0)
    {
        perror("listen() failed!");
        close(listenfd);
        return -1;
    }


    int epollfd = epoll_create(1);

    epoll_event evt;
    evt.data.fd = listenfd;
    evt.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &evt);

    epoll_event evtList[10];
    while (true)
    {
        int infds = epoll_wait(epollfd, evtList, 10, -1);
        if(infds < 0)
        {
            perror("epoll_wait()");
            break;
        }
        if(infds == 0)
        {
            cout << "wait timeout" << endl;
            continue;
        }

        for (int i = 0; i < infds; i++)
        {
            int curfd = evtList[i].data.fd;
            uint32_t events = evtList[i].events;
            if (events & EPOLLIN)
            {
                if (curfd == listenfd)
                {
                    // 有新的客户端连接
                    sockaddr_in peerAddr;
                    socklen_t sockLen = sizeof peerAddr;
                    int clientfd = accept4(listenfd, (sockaddr *)&peerAddr, &sockLen, O_NONBLOCK);
                    if (clientfd < 0)
                    {
                        perror("accept()");
                        continue;
                    }
                    
                    InetAddress clientAddr(peerAddr);
                    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, clientAddr.ip(), clientAddr.port());

                    evt.data.fd = clientfd;
                    evt.events = EPOLLIN | EPOLLET; // 边缘触发
                    // 将客户端添加到epoll 的监听中
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &evt);
                }
                else
                {
                    // 边缘触发需要循环读取
                    char buffer[1024];
                    while (true)
                    {
                        bzero(buffer, sizeof(buffer));

                        int readLen = recv(curfd, buffer, sizeof(buffer), 0);
                        if (readLen > 0)
                        {
                            printf("recv(clientfd=%d):%s\n", curfd, buffer);
                            send(curfd, buffer, readLen, 0);
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
                            printf("client(fd=%d) disconnect.\n", curfd);
                            close(curfd);
                            break;
                        }
                    }
                }
            }
            else if (events & EPOLLOUT)
            {
                // 写事件处理
            }
            else
            {
                // 其他事件都视为错误
                printf("client(fd=%d) error.\n", curfd);
                close(curfd);
            }
        }
    }

    return 0;
}
