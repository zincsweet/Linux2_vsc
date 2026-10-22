#pragma once
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <functional>
#include "Logger.hpp"
#include "InetAddr.hpp"


using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static uint16_t default_port = 8888;

// using callback_t = std::function<std::string(std::string)>;
using handler_addr_t = std::function<void (const InetAddr&)>;
using handler_msg_t = std::function<void (int, std::string)>;

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
    UdpServer(uint16_t port = default_port)
    :_sockfd(default_fd),
     _port(port)
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

        InetAddr local(_port);
        
        // 只是填充在了local中，并没有写入内核

        // 第三步：绑定套接字socket信息(写入内核)
        ssize_t r = bind(_sockfd, (struct sockaddr*)local.GetNetAddress(), local.Len());
        if (r < 0) {
            LOG(LogLevel::FATAL) << "绑定套接字失败：" << strerror(errno);
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "绑定套接字成功。" << " port:" << _port;
    }

    void RegisterService(handler_addr_t handler_addr, handler_msg_t handler_msg) {
        _handler_addr = handler_addr;
        _handler_msg = handler_msg;
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
                inbuffer[r] = 0;

                // 1. 检测新用户
                InetAddr client_address(peer);
                std::string message = client_address.ToString() + inbuffer;
                LOG(LogLevel::DEBUG) << message;                
                _handler_addr(client_address);

                // 2. 转发消息
                _handler_msg(_sockfd, message);
            }
        }
    }
private:
    int _sockfd;
    // std::string _ip;    // "192.168.2.2"(字符串风格的点分十进制IP地址，给人看) && 4字节IP
    uint16_t _port;
    // callback_t _cb;
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};