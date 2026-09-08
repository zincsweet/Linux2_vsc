#include "BlockQueue.hpp"
#include <unistd.h>
#include <memory>
#include "thread.hpp"



int main() {
    std::unique_ptr<BlockQueue<int> > bq = std::make_unique<BlockQueue<int> >();

    my::Thread comsumer([&bq](){
        while (true) {
            sleep(1);
            int data;
            bq->Pop(&data);
            std::cout << "消费：" << data << std::endl;
        }
    });

    my::Thread productor([&bq](){
        int data = 10;
        while (true) {
            bq->Enqueue(data);
            std::cout << "生产：" << data++ << std::endl;
        }
    });

    comsumer.Start();
    productor.Start();

    comsumer.Join();
    productor.Join();

    return 0;
}