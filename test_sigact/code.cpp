#include <iostream>
#include <unistd.h>
#include <signal.h>

void handler(int signo) {
    std::cout << "捕捉了一个信号:" << signo << std::endl;
    sigset_t pending;
    while (true) {
        sigpending(&pending);
        for (int i = 31; i >= 1; --i) {
            if (sigismember(&pending, i)) {
                std::cout << "1";
            }
            else {
                std::cout << "0";
            }
        }
        sleep(1);
        std::cout << std::endl;
    }
}

int main() {

    struct sigaction act, oact;
    act.sa_handler = handler;
    sigaction(SIGINT, &act, &oact);

    while (true) {
        std::cout << "进程正在运行:" << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}