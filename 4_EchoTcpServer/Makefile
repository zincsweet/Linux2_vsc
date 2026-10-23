.PHONY:all
all:client_tcp server_tcp
server_tcp:EchoTcpServerMain.cpp
	g++ -o $@ $^ -std=c++17
client_tcp:EchoTcpClient.cpp
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f client_tcp server_tcp