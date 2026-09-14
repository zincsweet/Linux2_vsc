#include "ThreadPool.hpp"
#include "Logger.hpp"
#include <memory>
#include <functional>
#include <ctime>
#include <cstdio>

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL_MODULE;

// using task_t = std::function<void ()>;

class Task
{
public:
    Task()
    {}
    Task(int x, int y) : _x(x), _y(y)
    {}
    ~Task()
    {}

    void operator()() {
        _result = _x + _y;
    }

    std::string Result() {
        return std::to_string(_x) + " + " + std::to_string(_y) + " = " + std::to_string(_result);
    }
private:
    int _x;
    int _y;
    int _result;
};


int main() {
    ENABLE_CONSOLE_LOG_STRATEGY();
    srand(time(nullptr) ^ getpid());
    std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>();
    tp->Start();
    int cnt = 10;
    while (cnt--) {
        int x = rand() % 10 + 1;
        usleep(135);
        int y = rand() % 20 + 1;
        Task t(x, y);

        // tp->Enqueue([](){
        //     LOG(LogLevel::DEBUG) << "我是一个任务，正在被处理...";
        // });
        // sleep(1);

        tp->Enqueue(t);
        sleep(1);
    }
    tp->Stop();
    tp->Wait();
    sleep(3);

    return 0;
}