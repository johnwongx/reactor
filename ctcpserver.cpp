#include <signal.h>

#include <functional>
#include <iostream>
#include <vector>

#include "EchoServer.h"
#include "utils/ThreadPool.h"

class MyClass /*: public std::enable_shared_from_this<MyClass>*/ {
 public:
  MyClass() { std::cout << "Construction func" << std::endl; }
  MyClass(const MyClass& rhs) { std::cout << "Copy construct" << std::endl; }
  ~MyClass() {}
};

void foo(const MyClass& data) {}

void test() {
  MyClass data;
  auto fn = std::bind(&foo, data);
}

EchoServer* srv = nullptr;
void StopServer(int sig) {
  delete srv;
  exit(0);
}

int main(int argc, char* argv[]) {
  // test();
  // return 0;

  signal(SIGINT, StopServer);
  signal(SIGTERM, StopServer);

  const unsigned short port = 5005;
  srv = new EchoServer("127.0.0.1", port, 3);
  srv->Start();

  return 0;
}
