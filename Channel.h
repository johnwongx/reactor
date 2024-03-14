#pragma once
#include <stdint.h>
#include <functional>

#include "Epoll.h"
#include "Socket.h"

// 处理事件函数
typedef std::function<bool(void)> ProcEvtFunc;

// 记录与处理事件
class Channel
{
public:
    Channel(int fd, Epoll* epoll, bool isListen):fd_(fd), epoll_(epoll) {
    }
    ~Channel(){
    }

    int fd() const { 
        return fd_;
    }
    
    void setInEpoll(bool in){
        inEpoll_ = in;
    }
    bool inEpoll() const{
        return inEpoll_;
    }

    void setEvents(uint32_t events){
        events_ = events;
    }
    void appendEvent(uint32_t events){
        events_ |= events;
    }
    uint32_t events() const{
        return events_;
    }

    void setREvents(uint32_t events){
        rEvents_ = events;
    }
    uint32_t rEvents() const{
        return rEvents_;
    }

    void setProcessInEvtFunc(ProcEvtFunc fn){
        inEvtFunc_ = fn;
    }

    bool handleEvent();

    bool onNewConnection(Socket* servSock);
    bool onMessage();

private:
    const int fd_;

    Epoll* epoll_;
    bool inEpoll_;

    uint32_t events_;
    uint32_t rEvents_;

    std::function<bool()> inEvtFunc_;
};