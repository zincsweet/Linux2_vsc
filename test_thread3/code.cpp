#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include "task.hpp"

int g_val = 100;

void PrintName(const std::string &name) {
    printf("我是新线程: %s, tid: ox%lx, pid: %d, g_val: %d, &g_val: %p\n",\
            name.c_str(), pthread_self(), getpid(), g_val, &g_val);
}

void* Routine(void* args) {
    sleep(2);
    Task* t = static_cast<Task*>(args);
    t->Excute();
    std::cout << t->Result() << std::endl;
    
    pthread_exit(nullptr);

    // return nullptr;
    // printf("-----------------------------\n");
    // std::string name = static_cast<const char*>(args);  //目标类型 变量 = static_cast<目标类型>(要转换的值);
    // while (true) {
    //     std::cout << "我是新线程:" << name << std::endl;
    //     PrintName(name);
    //     sleep(1);
    //     ++g_val;
    // }
}

int main() {
    srand(time(nullptr) ^ getpid());
    // 创建多线程
    const int thread_count = 10;
    for(int i = 0; i < thread_count; ++i) {
        pthread_t tid;
        // 设置名称
        // char thread_name[64];   // 可能发生重复访问
        // snprintf(thread_name, sizeof(thread_name), "thread-%d", i + 1);
        int x = rand() % 10 + 1;
        usleep(123);
        int y = rand() % 7 + 1;
        Task* t = new Task(x, y);

        int ret = pthread_create(&tid, nullptr, Routine, t);
        if (ret != 0) {
            std::cerr << "pthread_create失败:" << strerror(ret) << std::endl;
            exit(1);
        }
        // sleep(1);
    }

    while (true) {
        printf("我是主线程tid: 0x%lx, pid: %d, g_val: %d, &g_val: %p\n",\
                pthread_self(), getpid(), g_val, &g_val);
        sleep(1);
    }

    return 0;
}