all:client server

client:../ctcpclient.cpp 
	g++ -o client ../ctcpclient.cpp -g


server:../ctcpserver.cpp ../InetAddress.cpp ../Socket.cpp ../Epoll.cpp ../Channel.cpp ../EventLoop.cpp
	g++ -o server ../ctcpserver.cpp ../InetAddress.cpp ../Socket.cpp ../Epoll.cpp ../Channel.cpp ../EventLoop.cpp -g

clean:
	rm -f client server
