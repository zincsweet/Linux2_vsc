#include <iostream>
#include <unistd.h>
#include <signal.h>

// signo是被signal中的2传递进来的
void hander(int signo) {
    std::cout << "收到了一个信号:" << signo << "执行自定义操作" << std::endl;
    exit(10);
}

int main() {

    signal(2, hander);
    signal(3, hander);

    while (true) {
        std::cout << "test sig..." << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}