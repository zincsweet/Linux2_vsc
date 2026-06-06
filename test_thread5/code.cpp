#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>

__thread char buf[1024];

void* Routine(void* args) {
    int num = *(int*)args;
    snprintf(buf, sizeof(buf), "线程 %d 正在写入数据进buf...", num);
    printf("%s\n", buf);
    return nullptr;
}

int main() {
    pthread_t tid1, tid2;
    int t1 = 1;
    int t2 = 2;
    pthread_create(&tid1, nullptr, Routine, &t1);
    pthread_create(&tid2, nullptr, Routine, &t2);

    void* ret1;
    void* ret2;
    pthread_join(tid1, &ret1);
    pthread_join(tid2, &ret2);

    return 0;
}