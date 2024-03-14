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
#include "EventLoop.h"
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

    EventLoop loop;
    // TODO: 释放资源
    Channel* servChan = new Channel(listenfd, &loop, true);
    servChan->appendEvent(EPOLLIN);
    servChan->setProcessInEvtFunc(std::bind(&Channel::onNewConnection, servChan, &servSoc));
    loop.addChannel(servChan);

    loop.run();

    return 0;
}
