#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void Usage(const std::string &proc) {
    std::cout << "Usage:\n\t";
    std::cout << proc << " [server_ip] [server_port]" << std::endl;
}


int main(int argc, char* argv[]) {

    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    // 1. 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket error:" << strerror(errno);
        exit(2);
    }

    // 2. 不需要显示的去bind绑定addr（IP+port）信息，udp client首次发送数据的时候，OS底层会隐式自动获取随机端口并绑定IP+port
    
    // 后面发数据的时候，只需要sockaddr就行
    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server.sin_addr);

    // 3. 循环执行客户端
    while (true) {

        std::string message;
        // 1. 获取用户输入
        std::cout << "请输入要发送的消息:";
        std::getline(std::cin, message);

        // 2. sendto发送数据给server
        ssize_t r = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));
        if (r > 0) {
            // recvform
            char inbuffer[1024] = {0};
            struct sockaddr_in tmp{};   // 避免recvfrom()内参数填NULL，可能会出问题，只是存放收到的服务端的套接字
            socklen_t tmplen = sizeof(tmp);
            ssize_t n = recvfrom(sockfd, inbuffer, sizeof(inbuffer), 0, (struct sockaddr*)&tmp, &tmplen);
            if (n > 0) {
                inbuffer[n] = 0;
                std::cout << inbuffer << std::endl;
            }
        }
        else {
            std::cerr << "sendto seeor:" << strerror(errno);
            exit(3);
        }

    }


    
    return 0;
}