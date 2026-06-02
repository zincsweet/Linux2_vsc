#include <iostream>
#include <signal.h>
#include <cstring>

void hander(int signo) {
    std::cout << "进程捕捉到信号:" << signo << "; 捕捉进程pid:" << getpid() << std::endl;
}

int main() {
    signal(2, hander);
    while (true) {
        std::cout << "进程正在运行:" << getpid() << std::endl;
        sleep(2);
        int r = raise(2);
        if (r == -1) {
            std::cerr << "raise失败:" << strerror(errno) << std::endl;
            exit(1);
        }
    }
}