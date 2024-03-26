#include <iostream>
#include <vector>

#include "EchoServer.h"
#include "utils/ThreadPool.h"

class MyClass : public std::enable_shared_from_this<MyClass> {};

void test() {
  std::shared_ptr<MyClass> data = std::make_shared<MyClass>();
  {
    void* p = data.get();
    std::shared_ptr<MyClass> tmp = ((MyClass*)p)->shared_from_this();
  }
}

int main(int argc, char* argv[]) {
  // test();
  // return 0;

  const unsigned short port = 5005;
  EchoServer srv("127.0.0.1", port);
  srv.Start();

  return 0;
}
