#pragma once

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

const static uint16_t default_port = 8888;

enum {
    SUCCESS,
    USE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR
};

class TcpServer
{
public:
    TcpServer(uint16_t port = default_port)
    :_port(port)
    {}
    ~TcpServer()
    {
        close(_listen_sockfd);
    }

    void Init() {
        // 1. 创建套接字
        _listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listen_sockfd < 0) {
            LOG(LogLevel::FATAL) << "套接字创建失败！";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "套接字创建成功！";

        // 2. 填充本地socket信息(填成网络)
        struct sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY; // 当要监听所有网卡时就用老办法

        // 3. bind绑定
        int n = bind(_listen_sockfd, (struct sockaddr*)&local, sizeof(local));
        if (n < 0) {
            LOG(LogLevel::FATAL) << "bind绑定失败!";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind绑定成功!";

        // 4. TCP面向连接，所以TCP服务器要一直处于监听(listen)状态
        int r = listen(_listen_sockfd, SOMAXCONN);
        if (r < 0) {
            LOG(LogLevel::FATAL) << "listen监听失败!";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen监听开启!";
        
    }

    void Loop() {
        while (true) {
            struct sockaddr_in clientaddr;
            socklen_t clientaddr_len = sizeof(clientaddr);
            
            // 5. 获取链接
            int sockfd = accept(_listen_sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
            if (sockfd < 0) {
                LOG(LogLevel::WARNING) << "accept失败，跳过此请求。";
                continue;
            }
            LOG(LogLevel::INFO) << "accpet成功!，sockfd:" << sockfd;

            // 6. 处理新的sockfd


            close(sockfd);
        }

        
    }

private:
    int _listen_sockfd;
    uint16_t _port;
};