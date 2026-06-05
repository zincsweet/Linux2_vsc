#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>

void* Routine(void* args) {
    std::string name = static_cast<const char*>(args);  //目标类型 变量 = static_cast<目标类型>(要转换的值);
    while (true) {
        // std::cout << "我是新线程:" << name << std::endl;
        printf("我是新线程: %s, tid: ox%lx\n", name.c_str(), pthread_self());
        sleep(1);
    }
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, Routine, (void*)"thread-1");
    if (ret != 0) {
        std::cerr << "pthread_create错误:" << strerror(ret) << std::endl;
        exit(1);
    }

    printf("new thread tid: 0x%lx\n", tid);

    while (true) {
        std::cout << "我是主线程...." << std::endl;
        sleep(1);
    }

    return 0;
}