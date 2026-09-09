#include "RingQueue.hpp"
#include <unistd.h>
#include "Task.hpp"

Mutex cnt_lock;
Mutex screen_lock;

int data = 1;

int GetData() {
    cnt_lock.Lock();
    int result = data++;
    cnt_lock.UnLock();
    return result;
}

void Print(const std::string& info) {
    screen_lock.Lock();
    std::cout << info << std::endl;
    screen_lock.UnLock();
}

void* ConsumerRoutine(void* args) {
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);

    int data = 0;
    while (true) {
        sleep(1);
        rq->Pop(&data);
        Print("消费数据：" + std::to_string(data));
    }
}

void* ProductorRoutine(void* args) {
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);

    while (true) {
        int data = GetData();
        rq->Enqueue(data);
        Print("生产数据：" + std::to_string(data));
    }
}

int main() {
    RingQueue<int>* rq = new RingQueue<int>();

    // 单单
    // pthread_t c, p;
    // pthread_create(&c, nullptr, ConsumerRoutine, rq);
    // pthread_create(&p, nullptr, ProductorRoutine, rq);

    // pthread_join(c, nullptr);
    // pthread_join(p, nullptr);

    // 多多
    pthread_t c[2], p[3];
    pthread_create(c, nullptr, ConsumerRoutine, rq);
    pthread_create(c+1, nullptr, ConsumerRoutine, rq);
    pthread_create(p, nullptr, ProductorRoutine, rq);
    pthread_create(p+1, nullptr, ProductorRoutine, rq);
    pthread_create(p+2, nullptr, ProductorRoutine, rq);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);
    return 0;
}