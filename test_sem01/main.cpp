#include "RingQueue.hpp"
#include <unistd.h>
#include "Task.hpp"
#include <ctime>
#include <cstdlib>

// Mutex cnt_lock;
// Mutex screen_lock;

// int data = 1;

// int GetData() {
//     cnt_lock.Lock();
//     int result = data++;
//     cnt_lock.UnLock();
//     return result;
// }

// void Print(const std::string& info) {
//     screen_lock.Lock();
//     std::cout << info << std::endl;
//     screen_lock.UnLock();
// }

void* ConsumerRoutine(void* args) {
    // RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);
    RingQueue<Task>* rq = static_cast<RingQueue<Task>*>(args);

    int data = 0;
    while (true) {
        // sleep(1);
        // rq->Pop(&data);
        // Print("消费数据：" + std::to_string(data));

        sleep(1);
        Task t;
        rq->Pop(&t);
        t();
        std::cout << "消费并完成任务：" << t.getResult() << std::endl;
    }
}

void* ProductorRoutine(void* args) {
    // RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);
    RingQueue<Task>* rq = static_cast<RingQueue<Task>*>(args);

    while (true) {
        // int data = GetData();
        // rq->Enqueue(data);
        // Print("生产数据：" + std::to_string(data));
        
        int x = rand() % 10 + 1;
        usleep(135);
        int y = rand() % 10 + 1;
        Task t(x, y);
        rq->Enqueue(t);
        std::cout << "生产任务：" << t.Question() << std::endl;
    }
}

int main() {
    srand(time(nullptr)^getpid());
    // RingQueue<int>* rq = new RingQueue<int>();
    RingQueue<Task>* rq = new RingQueue<Task>();

    // 单单
    pthread_t c, p;
    pthread_create(&c, nullptr, ConsumerRoutine, rq);
    pthread_create(&p, nullptr, ProductorRoutine, rq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    // 多多
    // pthread_t c[2], p[3];
    // pthread_create(c, nullptr, ConsumerRoutine, rq);
    // pthread_create(c+1, nullptr, ConsumerRoutine, rq);
    // pthread_create(p, nullptr, ProductorRoutine, rq);
    // pthread_create(p+1, nullptr, ProductorRoutine, rq);
    // pthread_create(p+2, nullptr, ProductorRoutine, rq);

    // pthread_join(c[0], nullptr);
    // pthread_join(c[1], nullptr);
    // pthread_join(p[0], nullptr);
    // pthread_join(p[1], nullptr);
    // pthread_join(p[2], nullptr);
    return 0;
}