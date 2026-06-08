#include "thread.hpp"
#include <unistd.h>
#include <vector>
#include <time.h>


class Task
{

public:
    Task(int x, int y): _x(x), _y(y){}
    ~Task(){}
    void Execute() {
        _result = _x + _y;
    }

    void Print() {
        std::cout << _x << " + " << _y << " = " << _result << std::endl;
    }

private:
    int _x;
    int _y;
    int _result;
};

void Loop() {
    char name[64];
    pthread_getname_np(pthread_self(), name, sizeof(name));
    int cnt = 5;
    while (cnt--) {
        std::cout << "new thread running:" << name << "  cnt:" << cnt << std::endl;
        sleep(1);
    }
}

void test01() {
    my::Thread t(Loop);
    sleep(1);
    t.Start();

    t.Detach();

    sleep(5);

    t.Stop();
    sleep(1);

    t.Join();
    t.PrintInfo();
}

void test02() {

    const int cnt = 10;
    srand(time(nullptr)^getpid());
    std::vector<Task> tasks;
    for(int i = 0; i < cnt; ++i) {
        tasks.emplace_back(rand() % 10 + 1, 3);
    }

    std::vector<my::Thread> threads;
    for (int i = 0; i < cnt; ++i) {
        threads.emplace_back([i, &tasks](){tasks[i].Execute();});
    }
    for (auto &t : threads) {
        t.Start();
        // sleep(1);
    }
    // for (auto &t : threads) {
    //     t.Stop();
    // }
    for (auto &t : threads) {
        t.Join();
        t.PrintInfo();
        // sleep(1);
    }
    for (auto &tk : tasks) {
        tk.Print();
    }
}



int main() {
    // test01();
    test02();
    return 0;
}