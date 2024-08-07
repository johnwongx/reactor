# Reactor
模仿muduo实现的reactor回声服务器。

类图：

![](https://github.com/johnwongx/reactor/blob/master/doc/reactor.png)

网络处理内部实际分为epoll部分的事件监听与发现，Epoll与Channel负责处理。这就相当于fd有两个侧面，其中一个封装为Socket，另一个封装为Channel.

Connector负责具体的数据收发。
Acceptor负责新连接建立。
这两个对象在收到事件时组装或解析数据，然后将加工好的数据交给外部处理。



消息接收流程：Epoll -> EventLoop->Channel::handleEvent->Connector->::recv->TcpServer->EchoServer(工作线程异步处理）



新连接流程：Epoll -> EventLoop->Channel->Acceptor->
TcpServer(通过策略分配到某一个EventLoop中）->EchoServer
