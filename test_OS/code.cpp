#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <vector>

int current = 0;

struct task_struct {
public:
    task_struct(int p)
        : _pid(p)
        , _counter(5)
    {}
    ~task_struct(){}


    void run() {
        std::cout << "process" << _pid << "running" << std::endl;
    }

    void desc() {
        --_counter;
    }

    bool Expired() {
        return _counter <= 0;
    }

    int GetPid() {
        return _pid;
    }

    void ResetCounter() {
        _counter = 5;
    }

private:
    int _pid;
    int _status;
    int _counter;
};


std::vector<task_struct> tasks;

void do_timer(int signo) {
    tasks[current].desc();
    if (tasks[current].Expired()) {
        std::cout << "进程pid为" << tasks[current].GetPid() << "的进程过期了，重新选择中" << std::endl;
        // 选择一个进程进行
        current = rand()%tasks.size();
        tasks[current].ResetCounter();
    }
    else {
        tasks[current].run();
    }

    //reset
    alarm(1);
}


int main() {
    alarm(1);
    signal(SIGALRM, do_timer);
    srand((unsigned int)time(nullptr));

    tasks.emplace_back(1);
    tasks.emplace_back(2);
    tasks.emplace_back(3);
    tasks.emplace_back(4);
    tasks.emplace_back(5);


    for(;;) {
        printf("OS 被唤醒\n");
        pause();
    }
    return 0;
}