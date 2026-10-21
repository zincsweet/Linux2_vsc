#pragma once
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <functional>
#include "Logger.hpp"


using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static uint16_t default_port = 8888;

using callback_t = std::function<std::string(std::string)>;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR
};

class UdpServer
{
public:
    // UdpServer(const std::string &ip, uint16_t port = default_port)
    UdpServer(callback_t cb, uint16_t port = default_port)
    :_sockfd(default_fd),
     _port(port),
     _cb(cb)
    {}

    ~UdpServer()
    {
        close(_sockfd);
    }

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
        local.sin_port = htons(_port); // hton
        // inet_pton(AF_INET, _ip.c_str(), &local.sin_addr);
        local.sin_addr.s_addr = INADDR_ANY;

        /*
        一台机器（尤其云服务器）通常不止一个 IP：
        地址	|说明
        公网 IP	|云厂商分配的对外地址
        内网 IP	|比如 172.x.x.x / 10.x.x.x，VPC 内网用的
        回环	|127.0.0.1
        如果写成 local.sin_addr.s_addr = inet_addr("某个具体IP")，
        那这个 socket 就只监听那一个 IP 的端口。比如绑了公网 IP，内网 IP 来的包就收不到。
        而 INADDR_ANY（0.0.0.0）会让内核把 socket 挂到所有本地地址上，
        无论客户端走公网、内网还是回环，只要目标是这个端口，都能收到。
        对服务器来说这是默认、最省心的选择。
        */

        // local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 1.字符串ip->4字节ip; 2.hton
        
        
        // 只是填充在了local中，并没有写入内核

        // 第三步：绑定套接字socket信息(写入内核)
        ssize_t r = bind(_sockfd, (struct sockaddr*)&local, sizeof(local));
        if (r < 0) {
            LOG(LogLevel::FATAL) << "绑定套接字失败：" << strerror(errno);
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "绑定套接字成功。" << " port:" << _port;
    }

    void Loop() {
        char inbuffer[1024] = {0};
        while (true) {
            struct sockaddr_in peer;
            socklen_t peerlen = sizeof(peer);       
            ssize_t r = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&peer, &peerlen);
            if (r < 0) {
                LOG(LogLevel::ERROR) << "recvfrom接收失败:" << strerror(errno);
                continue;
            }
            else if (r == 0) {
                LOG(LogLevel::WARNING) << "recvfrom接收空数据。";
            }
            else {
                // 客户端的信息
                uint16_t client_port = ntohs(peer.sin_port);
                char client_ip_char[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &peer.sin_addr, client_ip_char, sizeof(client_ip_char));
                std::string client_ip(client_ip_char);
                std::string client_addr = "[" + client_ip + ":" + std::to_string(client_port) + "]";


                // 用户发来的数据
                inbuffer[r] = 0;
                // LOG(LogLevel::DEBUG) << "client [" << client_addr << "] say# " << inbuffer;
                // std::string echo_string = "server echo---";
                // echo_string += inbuffer;

                std::string result = _cb(inbuffer);
                
                sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr*)&peer, peerlen);
            }
        }
    }
private:
    int _sockfd;
    // std::string _ip;    // "192.168.2.2"(字符串风格的点分十进制IP地址，给人看) && 4字节IP
    uint16_t _port;
    callback_t _cb;
};