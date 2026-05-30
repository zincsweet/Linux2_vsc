#include <iostream>
#include "pipe.hpp"

int main() {
    // 创建管道 && 打开管道

    Fifo pipefifo;
    pipefifo.Build();
    pipefifo.Open(FIFO_READ);

    std::string msg;
    while (true) {
        int n = pipefifo.Recv(msg);
        if (n == -1) {
            std::cerr << "client:接收消息失败" << std::endl;
            break;
        }
        else if (n == 0) {
            std::cout << "client:管道已关闭" << std::endl;
            break;
        }
        else {
            std::cout << "client:收到消息: " << msg << std::endl;
        }
    }


    pipefifo.Destroy();

    return 0;
}