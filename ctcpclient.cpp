#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>

using namespace std;

class ctcpclient
{
public:
    ctcpclient() : m_sockfd(-1)
    {
    }
    ~ctcpclient()
    {
        close();
    }

    bool connect(const std::string &ip, const unsigned short port)
    {
        if (m_sockfd != -1)
            return false;

        // 第1步：创建客户端的socket。
        m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sockfd == -1)
            return false;

        // 第2步：向服务器发起连接请求。
        struct sockaddr_in servaddr; // 用于存放协议、端口和IP地址的结构体。
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;   // ①协议族，固定填AF_INET。
        servaddr.sin_port = htons(port); // ②指定服务端的通信端口。

        struct hostent *h = gethostbyname(ip.data()); // 用于存放服务端IP地址(大端序)的结构体的指针。
        if (!h)                                       // 把域名/主机名/字符串格式的IP转换成结构体。
        {
            close();
            return false;
        }
        memcpy(&servaddr.sin_addr, h->h_addr, h->h_length); // ③指定服务端的IP(大端序)。

        if (::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) // 向服务端发起连接清求。
        {
            close();
            return false;
        }

        return true;
    }

    bool send(const string &info)
    {
        if(m_sockfd == -1)
            return false;

        int iret = ::send(m_sockfd, info.data(), info.size(), 0);
        if (iret <= 0)
            return false;
        return true;
    }

    bool send(const void* buffer, int size)
    {
        if(m_sockfd == -1)
            return false;

        int iret = ::send(m_sockfd, buffer, size, 0);
        if (iret <= 0)
            return false;

        cout << "send success!" << endl;
        return true;
    }

    bool recv(string& buffer, int maxLen)
    {
        if(m_sockfd == -1)
            return false;

        buffer.clear();
        buffer.resize(maxLen);
        int readn = ::recv(m_sockfd, &buffer[0], buffer.size(), 0);
        if (readn <= 0)
        {
            buffer.clear();
            return false;
        }
        buffer.resize(readn);
        
        return true;
    }

    bool close()
    {
        if (m_sockfd == -1)
            return false;

        ::close(m_sockfd);
        m_sockfd = -1;
        return true;
    }

public:
    int m_sockfd;
    std::string m_ip;
    unsigned short m_port;
};

int main(int argc, char *argv[])
{
    string ip = "localhost";
    const unsigned short port = 5005;

    ctcpclient c;
    if(!c.connect(ip, port))
    {
        perror("connect() failed!");
        return -1;
    }

    char buffer[1024];
    for (int i = 0; i < 10; i++)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("please input:");
        scanf("%s", buffer);
        if(!c.send(buffer))
        {
            perror("send()");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        if(recv(c.m_sockfd, buffer, sizeof(buffer), 0) <= 0)
        {
            perror("recv()");
            break;
        }
        printf("recv:%s\n", buffer);
    }
    
    return 0;
}