.PHONY:all
all:client_chat server_chat
server_chat:ChatServerMain.cpp
	g++ -o $@ $^ -std=c++17
client_chat:ChatClient.cpp
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f client_chat server_chat