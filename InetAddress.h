#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
public:
    InetAddress(const std::string& ip, uint16_t port);
    InetAddress(sockaddr_in addr);
    InetAddress();
    ~InetAddress();

    void setAddr(const sockaddr_in& addr);
    const sockaddr* addr() const;

    // 返回主机序列
    const char* ip() const;
    uint16_t port() const;

private:
    sockaddr_in addr_;
};