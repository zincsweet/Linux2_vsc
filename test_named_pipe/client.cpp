#include <iostream>
#include "pipe.hpp"

int main() {
    // 打开管道
    Fifo fileclient;
    fileclient.Open(FIFO_WRITE);
    while (true) {
        std::cout << "Client@请输入要发送的消息: ";
        std::string msg;
        std::getline(std::cin, msg);
        fileclient.Send(msg);
    }
    return 0;
}