#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "thread.hpp"
#include "InetAddr.hpp"


using namespace NS_THREAD_MODULE;

int sockfd = 0;
std::string server_ip;
uint16_t server_port = 0;
std::string nickname;

void Usage(const std::string &proc) {
    std::cout << "Usage:\n\t";
    std::cout << proc << " [server_ip] [server_port]" << std::endl;
}

void Online(InetAddr& server_addr) {
    std::cout << "请设置你的昵称：";
    std::getline(std::cin, nickname);
    std::string online_message = nickname + " 上线了！";
    ssize_t r = sendto(sockfd, online_message.c_str(), online_message.size(), 0, (struct sockaddr*)server_addr.GetNetAddress(), server_addr.Len());
    (void)r;
}

void RecvMessage() {
    while (true) {
        // recvform
        char inbuffer[1024] = {0};
        struct sockaddr_in tmp{};   // 避免recvfrom()内参数填NULL，可能会出问题，只是存放收到的服务端的套接字
        socklen_t tmplen = sizeof(tmp);
        ssize_t n = recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&tmp, &tmplen);
        if (n > 0) {
            inbuffer[n] = 0;
            std::cout << inbuffer << std::endl;
        }
    }
}


void SendMessage() {
    InetAddr server_addr(server_port, server_ip);
    Online(server_addr);


    while (true) {
        std::string message;
        // 1. 获取用户输入
        std::cout << "请输入要发送的消息:";
        std::getline(std::cin, message);
        message = nickname + "$ " + message;

        // 2. sendto发送数据给server
        ssize_t r = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)server_addr.GetNetAddress(), server_addr.Len());
        (void)r;
    }
}


int main(int argc, char* argv[]) {

    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }

    server_ip = argv[1];
    server_port = std::stoi(argv[2]);

    // 1. 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket error:" << strerror(errno);
        exit(2);
    }

    // 2. 线程模块
    Thread recver(RecvMessage);
    Thread sender(SendMessage);
    recver.Start();
    sender.Start();

    recver.Join();
    sender.Join();

    return 0;
}
