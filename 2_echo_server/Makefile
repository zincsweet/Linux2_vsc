.PHONY:all
all:client_udp server_udp
server_udp:EchoServerMain.cpp
	g++ -o $@ $^ -std=c++17
client_udp:EchoClient.cpp
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f client_udp server_udp