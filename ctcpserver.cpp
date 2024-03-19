#include "TcpServer.h"

int main(int argc, char *argv[])
{
    const unsigned short port = 5005;
    TcpServer srv("127.0.0.1", port);
    srv.start();

    return 0;
}
