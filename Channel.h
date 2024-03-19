/*
用于处理fd发送过来的事件
*/
#pragma once
#include <stdint.h>
#include <functional>

class EventLoop;
class Socket;

// 处理事件函数
typedef std::function<bool(void)> ProcEvtFunc;

// 记录与处理事件
class Channel
{
public:
    Channel(int fd, EventLoop* loop, bool isListen):fd_(fd), loop_(loop) {
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

    bool onMessage();

private:
    const int fd_;

    EventLoop* loop_;
    bool inEpoll_;

    uint32_t events_;
    uint32_t rEvents_;

    std::function<bool()> inEvtFunc_;
};