all:client server

client:../ctcpclient.cpp ../Buffer.cpp
	g++ -o client ../ctcpclient.cpp ../Buffer.cpp -g


server:../ctcpserver.cpp ../InetAddress.cpp ../Socket.cpp ../Epoll.cpp ../Channel.cpp ../EventLoop.cpp ../TcpServer.cpp ../Acceptor.cpp ../Connector.cpp ../Buffer.cpp ../EchoServer.cpp
	g++ -o server ../ctcpserver.cpp ../InetAddress.cpp ../Socket.cpp ../Epoll.cpp ../Channel.cpp ../EventLoop.cpp ../TcpServer.cpp ../Acceptor.cpp ../Connector.cpp ../Buffer.cpp ../EchoServer.cpp -g

clean:
	rm -f client server
