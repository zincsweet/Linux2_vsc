#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include "InetAddr.hpp"

std::string server_ip;
uint16_t server_port = 0;

void Usage(const std::string &process) {
    std::cout << "Usage:\n\t";
    std::cout << process << "[server_ip]" << "[server_port]" << std::endl;
}


// ./client_tcp [server_ip] [server_port]
int main(int argc, char* argv[]) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }
    server_ip = argv[1];
    server_port = std::stoi(argv[2]);

    // 1. 创建tcp_socket套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "客户端socket()创建套接字失败。" << std::endl;
        exit(2);
    }

    // 2. 虽然不用显示bind()绑定，但是得connect()发起建立连接的请求
    InetAddr server_addr(server_port, server_ip);
    int n = connect(sockfd, (struct sockaddr*)server_addr.GetNetAddress(), server_addr.Len());
    if (n < 0) {
        std::cerr << "connect()连接服务端 " << server_addr.ToString() << " 错误！" << std::endl;
        exit(3);
    }
    std::cout << "connect()连接服务端成功！" << std::endl;

    // 3. 通信
    while (true) {
        std::string line;
        std::cout << "请输入需要发送的消息# " ;
        std::getline(std::cin, line);
        write(sockfd, line.c_str(), line.size());

        char inbuffer[1024];
        int r = read(sockfd, inbuffer, sizeof(inbuffer));
        if(r > 0) {
            inbuffer[r] = 0;
            std::cout << inbuffer << std::endl;
        }
        else if (r == 0) {
            std::cout << "未读到数据或已经读到文件末尾" << std::endl;
            break;
        }
        else {
            std::cerr << "read()读取错误" << std::endl;
            break;
        }

    }

    return 0;
}