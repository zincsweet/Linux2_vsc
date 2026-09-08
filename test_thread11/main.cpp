#include "BlockQueue.hpp"
#include <unistd.h>
#include <memory>
#include <ctime>
#include "thread.hpp"
#include "Task.hpp"



int main() {
    srand(time(nullptr) ^ getpid());
    std::unique_ptr<BlockQueue<Task> > bq = std::make_unique<BlockQueue<Task> >();

    my::Thread comsumer([&bq](){
        while (true) {
            sleep(1);
            Task t;
            // 1. 取数据
            bq->Pop(&t);
            // 2. 处理数据
            // t.Execute();
            t();
            std::cout << "消费：" << t.getResult() << std::endl;
        }
    });

    my::Thread productor([&bq](){
        while (true) {
            // 1. 获取数据
            int datax = rand() % 10 + 1;
            usleep(rand() % 1357);
            int datay = rand() % 10 + 1;

            // 2. 生产过程
            Task t(datax, datay);
            bq->Enqueue(t);
            std::cout << "生产：" << t.Question() << std::endl;
        }
    });

    comsumer.Start();
    productor.Start();

    comsumer.Join();
    productor.Join();

    return 0;
}