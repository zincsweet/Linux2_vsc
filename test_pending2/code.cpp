#include <iostream>
#include <cstring>
#include <signal.h>
#include <unistd.h>

void handler(int signo) {
    std::cout << "处理了" << signo << "号信号" << std::endl;
}

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
    // 0. 捕捉2号信号
    signal(2, handler);

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

    std::cout << "进程pid:" << getpid() << std::endl;

    int cnt = 0;
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

        if (cnt == 15) {
            // 4. 解除2号信号的屏蔽
            int r = sigprocmask(SIG_SETMASK, &old_set, nullptr);
            if (r == -1) {
                std::cerr << "sigprocmask解除2号屏蔽失败:" << strerror(errno) << std::endl;
                exit(3);
            }
            std::cout << "2号信号屏蔽已解除" << std::endl;
        }
        ++cnt;
        sleep(1);
    }
    return 0;
}