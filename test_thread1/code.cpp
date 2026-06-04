#include <iostream>
#include <unistd.h>
#include <pthread.h>

void* threadRun(void* arg) {
    while (true) {
        std::cout << "new thread is running! pid:" << getpid() << std::endl;
        sleep(1);
    }
}

int main() {
    pthread_t tid;
    pthread_create(&tid, nullptr, threadRun, nullptr);

    while (true) {
        std::cout << "main thread is running! pid" << getpid() << std::endl;
        sleep(1);
    }
    return 0;
}