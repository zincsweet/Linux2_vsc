#pragma once

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
// #include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <cstring>
#include "Logger.hpp"
#include "InetAddr.hpp"

using namespace NS_LOG_MODULE;

const static uint16_t default_port = 8888;

enum {
    SUCCESS,
    USE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    FORK_ERR
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

    void serviceIO(int sockfd, InetAddr& address) {

        // 短连接，短服务
        LOG(LogLevel::INFO) << "client address is " << address.ToString();
        char inbuffer[1024];
        // 读
        ssize_t r = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

        if (r > 0) {
            // 1. 收到数据
            inbuffer[r] = 0;
            LOG(LogLevel::INFO) << "数据接收成功,客户端 " << address.ToString() << " 说# " << inbuffer;
            // 写
            std::string echo_string = "server 回显# ";
            echo_string += inbuffer;
            write(sockfd, echo_string.c_str(), echo_string.size());
        }
        else if (r == 0) {
            // 2. 客户端主动断开连接
            LOG(LogLevel::INFO) << "客户端 " << address.ToString() << " 主动退出.";
        }
        else {  // n == -1
            // 3. 连接失败
            LOG(LogLevel::WARNING) << "客户端 " << address.ToString() << " 连接错误---" << strerror(errno);
        }


        // // 长服务，长连接，小型应用
        // LOG(LogLevel::INFO) << "client address is " << address.ToString();
        // while (true) {
        //     char inbuffer[1024];
        //     // 读
        //     ssize_t r = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

        //     if (r > 0) {
        //         // 1. 收到数据
        //         inbuffer[r] = 0;
        //         LOG(LogLevel::INFO) << "数据接收成功,客户端 " << address.ToString() << " 说# " << inbuffer;
        //         // 写
        //         std::string echo_string = "server 回显# ";
        //         echo_string += inbuffer;
        //         write(sockfd, echo_string.c_str(), echo_string.size());
        //     }
        //     else if (r == 0) {
        //         // 2. 客户端主动断开连接
        //         LOG(LogLevel::INFO) << "客户端 " << address.ToString() << " 主动退出.";
        //         break;
        //     }
        //     else {  // n == -1
        //         // 3. 连接失败
        //         LOG(LogLevel::WARNING) << "客户端 " << address.ToString() << " 连接错误---" << strerror(errno);
        //         break;
        //     }
        // }
    }

    class ThreadData
    {
    public:
        ThreadData(TcpServer* ts, int sockfd, const InetAddr& addr)
        :_this(ts),
         _sockfd(sockfd),
         _addr(addr)
        {}
        ~ThreadData()
        {
            close(_sockfd);
        }
    public:
        TcpServer* _this;
        int _sockfd;
        InetAddr _addr;
    };

    static void *thread_routine(void* args) {
        ThreadData* td = static_cast<ThreadData*>(args);
        pthread_detach(pthread_self());
        td->_this->serviceIO(td->_sockfd, td->_addr);
        delete td;
        return nullptr;
    }

    void Loop() {
        // signal(SIGCHLD, SIG_IGN);    非阻塞最佳实践
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
            
            // 第一版，单进程
            // InetAddr clientaddress(clientaddr);
            // serviceIO(sockfd, clientaddress);

            
            // 第二版，多进程
            // pid_t id = fork();
            // if (id < 0) {
            //     LOG(LogLevel::FATAL) << "fork error!";
            //     exit(FORK_ERR);
            // }
            // else if (id == 0) {
            //     // 子进程

            //     // 关闭掉自己不需要的sockfd
            //     close(_listen_sockfd);

            //     if (fork() > 0) exit(0);    // 子进程直接退出

            //     // 孙子进程 -- 孤儿进程(系统自动回收)
            //     InetAddr clientaddress(clientaddr);
            //     serviceIO(sockfd, clientaddress);
            //     close(sockfd);
            //     exit(0);
            // }
            // else {  // id > 0
            //     // 父进程

            //     // 关掉自己不需要的sockfd
            //     close(sockfd);

            //     pid_t rid = waitpid(id, nullptr, 0);
            //     (void)rid;
            // }


            // 第三版，多线程
            pthread_t tid;
            InetAddr clientaddress(clientaddr);
            ThreadData* td = new ThreadData(this, sockfd, clientaddress);
            pthread_create(&tid, nullptr, thread_routine, (void*)td);
            // 多线程下这里绝不能 close(sockfd)！
            // 所有线程共享同一张 fd 表，主线程关了工作线程就没法用了（Bad file descriptor）
        }
    }

private:
    int _listen_sockfd;
    uint16_t _port;
};