#include <iostream>
#include <vector>

#include "EchoServer.h"
#include "utils/ThreadPool.h"

int main(int argc, char* argv[]) {
  const unsigned short port = 5005;
  EchoServer srv("127.0.0.1", port);
  srv.Start();

  return 0;
}
