#include <iostream>
#include <cstring>
#include <signal.h>
#include <unistd.h>

void PrintPending(const sigset_t pending) {
    for (int signo = 31; signo > 0; --signo) {
        if (sigismember(&pending, signo)) {
            std::cout << "1";
        }
        else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

int main() {
    // 1. 屏蔽2号信号
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set, SIGINT);

    int r = sigprocmask(SIG_SETMASK, &block_set, &old_set);
    if (r == -1) {
        std::cerr << "sigprocmask失败:" << strerror(errno) << std::endl;
        exit(1);
    }

    while (true) {
        sigset_t pending;
        sigemptyset(&pending);

        // 2. 获取pending表
        int r = sigpending(&pending);
        if (r == -1) {
            std::cerr << "sigpending失败:" << strerror(errno) << std::endl;
            exit(2);
        }

        // 3. 打印pending表
        PrintPending(pending);
        sleep(2);
    }
    return 0;
}