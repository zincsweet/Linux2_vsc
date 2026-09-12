#include "ThreadPool.hpp"
#include "Logger.hpp"
#include <memory>
#include <functional>

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void ()>;


int main() {
    ENABLE_CONSOLE_LOG_STRATEGY();
    
    std::unique_ptr<ThreadPool<task_t>> tp = std::make_unique<ThreadPool<task_t>>();
    tp->Start();
    int cnt = 10;
    while (cnt--) {
        tp->Enqueue([](){
            LOG(LogLevel::DEBUG) << "我是一个任务，正在被处理...";
        });
        sleep(1);
    }
    tp->Stop();
    tp->Wait();
    sleep(3);

    return 0;
}