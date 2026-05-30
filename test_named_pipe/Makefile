.PHONY: all
all: client server

client: client.cpp
	g++ -o client client.cpp -std=c++11
server: server.cpp
	g++ -o server server.cpp -std=c++11

.PHONY: clean
clean:
	rm -f client server