#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "Logger.hpp"


using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static uint16_t default_port = 8888;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR
};

class UdpServer
{
public:
    UdpServer(const std::string &ip, uint16_t port = default_port)
    :_sockfd(default_fd),
     _ip(ip),
     _port(port)
    {}

    ~UdpServer()
    {}

    void Init() {
        // 第一步：创建socket，本质：打开网络这个文件
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0) {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success, sockfd: " << _sockfd;

        // 第二步：填充网络属性信息
        struct sockaddr_in local{}; // {}结构体清零
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = std::stoi(_ip);
        local.sin_port = _port;
        // 只是填充在了local中，并没有写入内核

        // 第三步：绑定网络信息
        

    }

    void Loop() {

    }
private:
    int _sockfd;
    std::string _ip;    // "192.168.2.2"(字符串风格的点分十进制IP地址，给人看) && 4字节IP
    uint16_t _port;
};