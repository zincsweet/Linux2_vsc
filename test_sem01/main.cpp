#include "RingQueue.hpp"
#include <unistd.h>

void* ConsumerRoutine(void* args) {
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);

    int data = 0;
    while (true) {
        sleep(1);
        rq->Pop(&data);
        std::cout << "消费：" << data << std::endl;
    }
}

void* ProductorRoutine(void* args) {
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);

    int data = 1;
    while (true) {
        rq->Enqueue(data);
        std::cout << "生产：" << data++ << std::endl;
    }
}

int main() {
    RingQueue<int>* rq = new RingQueue<int>();

    // 单单
    pthread_t c, p;
    pthread_create(&c, nullptr, ConsumerRoutine, rq);
    pthread_create(&p, nullptr, ProductorRoutine, rq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);


    return 0;
}