#pragma once
#include <stdint.h>

#include "Epoll.h"
#include "Socket.h"

// 记录与处理事件
class Channel
{
public:
    Channel(int fd, Epoll* epoll, bool isListen):fd_(fd), epoll_(epoll), isListen_(isListen){
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

    bool isListen() const {
        return isListen_;
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

    bool handleEvent(Socket& servSock);

private:
    const int fd_;

    Epoll* epoll_;
    bool inEpoll_;

    bool isListen_;

    uint32_t events_;
    uint32_t rEvents_;
};