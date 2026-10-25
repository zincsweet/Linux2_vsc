.PHONY:all
all:client_ssh server_ssh
server_ssh:SSH_Server.cpp
	g++ -o $@ $^ -std=c++17 -lpthread
client_ssh:SSH_Client.cpp
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f client_ssh server_ssh