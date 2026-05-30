#pragma once
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

const std::string gcommfile = "./fifo";

#define FIFO_READ 1
#define FIFO_WRITE 2

class Fifo {
public:
    Fifo(const std::string& commfile = gcommfile) 
        : _commfile(commfile),
        _mode(0666),
        _fd(-1) 
    {}
    
    // 1. 创建命名管道
    void Build() {
        if (IsExist()) {
            std::cout << "命名管道已存在" << std::endl;
            return;
        }

        int n = mkfifo(_commfile.c_str(), _mode);
        if (n == -1) {
            std::cerr << "创建命名管道失败: " << strerror(errno) << std::endl;  // strerror(errno) 获取错误信息
        } else {
            std::cout << "命名管道创建成功" << std::endl;
        }
    }

    // 2. 打开管道
    void Open(int open_mode) {
        // 管道为保障通信（读、写）双方同时存在，一端打开管道时会阻塞，直到另一方打开管道
        // 判断打开模式
        if (open_mode == FIFO_READ) {
            _fd = open(_commfile.c_str(), O_RDONLY);
        }
        else if (open_mode == FIFO_WRITE) {
            _fd = open(_commfile.c_str(), O_WRONLY);
        }
        else {
            std::cerr << "无效的打开模式" << std::endl;
            return;
        }
        
        if (_fd == -1) {
            std::cerr << "打开命名管道失败: " << strerror(errno) << std::endl;  // strerror(errno) 获取错误信息
        } else {
            std::cout << "命名管道打开成功" << std::endl;
        }
    }

    // 信息通信
    void Send(const std::string& msgin) {
        if (_fd == -1) {
            std::cerr << "管道未打开，无法发送消息" << std::endl;
            return;
        }

        ssize_t n = write(_fd, msgin.c_str(), msgin.size());
        if (n == -1) {
            std::cerr << "发送消息失败: " << strerror(errno) << std::endl;
        }
    }

    int Recv(std::string& msgout) {
        if (_fd == -1) {
            std::cerr << "管道未打开，无法接收消息" << std::endl;
            return -1;
        }

        char buffer[1024];
        ssize_t n = read(_fd, buffer, sizeof(buffer) - 1);
        if (n == -1) {
            // 接收消息失败
            return -1;
        } else if (n == 0) {
            // 管道已关闭
            return 0;
        }
        else {
            buffer[n] = '\0';   // n 是实际读取的字节数，确保字符串正确结束，用n下标设置'\0'
            msgout = buffer;
            return n;
        }
    }
    
    // 3. 销毁管道
    void Destroy() {
        // unlink() 函数用于删除一个文件或命名管道
        int n = unlink(_commfile.c_str());
        if (n == -1) {
            std::cerr << "销毁命名管道失败: " << strerror(errno) << std::endl;  // strerror(errno) 获取错误信息
        } else {
            std::cout << "命名管道销毁成功" << std::endl;
        }
    }


    ~Fifo() {}

private:
    bool IsExist() {
        // access() 函数用于检查文件是否存在
        return access(_commfile.c_str(), F_OK) == 0;
    }

private:
    std::string _commfile;
    mode_t _mode = 0666;
    int _fd;
};