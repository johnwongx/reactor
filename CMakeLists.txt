# 设置CMake的最小版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(NetworkProject VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置客户端和服务器的源文件
set(CLIENT_SOURCES
    ctcpclient.cpp
    Buffer.cpp
)
set(SERVER_SOURCES
    ctcpserver.cpp
    InetAddress.cpp
    Socket.cpp
    Epoll.cpp
    Channel.cpp
    EventLoop.cpp
    TcpServer.cpp
    Acceptor.cpp
    Connector.cpp
    Buffer.cpp
    EchoServer.cpp
    utils/ThreadPool.cpp
)

# 添加客户端和服务器可执行文件
add_executable(client ${CLIENT_SOURCES})
add_executable(server ${SERVER_SOURCES})

# 设置生成的可执行文件的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

# 指定编译选项
target_compile_options(client PRIVATE -g)
target_compile_options(server PRIVATE -g)

