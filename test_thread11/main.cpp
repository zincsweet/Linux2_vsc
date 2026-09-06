#include "BlockQueue.hpp"
#include <unistd.h>


void* ConsumerRoutine(void* args) {
    BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
    while (true) {
        sleep(5);
        int data;
        bq->Pop(&data);
        std::cout << "消费：" << data << std::endl;
    }
}

void* ProductorRoutine(void* args) {
    BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
    int data = 10;
    while (true) {
        bq->Enqueue(data);
        std::cout << "生产：" << data++ << std::endl;
    }
}


int main() {
    BlockQueue<int>* bq = new BlockQueue<int>();
    pthread_t c, p;
    pthread_create(&c, nullptr, ConsumerRoutine, bq);
    pthread_create(&p, nullptr, ProductorRoutine, bq);



    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    return 0;
}