#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include "task.hpp"


void* Routine(void* args) {
    pthread_detach(pthread_self());
    int cnt = 5;
    while (cnt) {
        std::cout << "新线程在运行，次数:" << cnt-- << std::endl;
        sleep(1);
        int a = 10;
        a /= 0;
    }
    return nullptr;
}

int main() {
    Task t(10, 20);
    pthread_t tid;
    pthread_create(&tid, nullptr, Routine, (void*)&t);

    sleep(100);
    // std::cout << "main 中detach分离线程" << std::endl;
    // pthread_detach(tid);

    // sleep(5);
    // std::cout << "取消目标线程..." << std::endl;
    // pthread_cancel(tid);

    // void* ret = nullptr;
    // int n = pthread_join(tid, &ret);
    // if (n != 0) {
    //     std::cerr << "join失败:" << strerror(n) << std::endl;
    //     exit(1);
    // }
    
    // std::cout << "join成功! 返回值:" << ret << std::endl;

    return 0;
}