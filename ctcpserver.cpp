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

#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"

using namespace std;

int main(int argc, char *argv[])
{
    const unsigned short port = 5005;

    int listenfd = Socket::createNonBlockSocket();
    Socket servSoc(listenfd);
    servSoc.setReuseAddr(true);
    servSoc.setNoDelay(true);

    InetAddress servAddr("127.0.0.1", port);
    servSoc.bind(servAddr);

    servSoc.listen();

    Epoll ep;
    // TODO: 释放资源
    Channel* servChan = new Channel(listenfd, &ep, true);
    servChan->appendEvent(EPOLLIN);
    servChan->setProcessInEvtFunc(std::bind(&Channel::onNewConnection, servChan, &servSoc));
    ep.addChannel(servChan);

    while (true)
    {
        std::vector<Channel*> chanList = ep.loop(-1);
        if(chanList.empty())
        {
            cout << "wait timeout" << endl;
            continue;
        }

        for(const auto& chanItr : chanList)
        {
            if (!chanItr->handleEvent())
            {
                if (chanItr->fd() == listenfd)
                {
                    std::cout << "listen fd error!" << std::endl;
                    exit(-1);
                }
                else
                {
                    std::cout << "client fd(" << chanItr->fd() << ")error!" << std::endl;
                    continue;
                }
            }
        }
    }

    return 0;
}
