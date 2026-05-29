#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <functional>

#define POLL_SIZE 4

// ***********************************任务列表*********************************************
void SyncDisk() {
    std::cout << "Syncing disk..." << std::endl;
    sleep(1); // 模拟同步磁盘的时间
    std::cout << "Disk synced." << std::endl;
}

void DownloadFile() {
    std::cout << "Downloading file..." << std::endl;
    sleep(1); // 模拟下载文件的时间
    std::cout << "File downloaded." << std::endl;
}

void PrintMessage() {
    std::cout << "Printing message..." << std::endl;
    sleep(1); // 模拟打印消息的时间
    std::cout << "Message printed." << std::endl;
}

void UpdateDatabase() {
    std::cout << "Updating database..." << std::endl;
    sleep(1); // 模拟更新数据库的时间
    std::cout << "Database updated." << std::endl;
}

typedef void (*task_t)();   // 定义一个函数指针类型，指向任务函数
task_t tasks[] = {SyncDisk, DownloadFile, PrintMessage, UpdateDatabase};  // 任务列表




// ***********************************进程池实现*******************************************

enum {
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};



void Dotask(int fd) {   // 任务的入口
    while (1) {
        sleep(1);
    }
}

// typedef std::function<void (int)> cb_t;
using cb_t = std::function<void (int)>;



class ProcessPool {

private:

    // 定义一个内部Channel类，保存管道写端和子进程pid
    class Channel {
    public:
        Channel(int wfd, pid_t sub_pid)
            : _wfd(wfd), _sub_pid(sub_pid)
        {
            _sub_name = "sub_channel_" + std::to_string(sub_pid);
        }

        ~Channel()
        {
        }

        void PrintInfo() {
            printf("Channel Info - Sub PID: %d, Sub Name: %s\n", _sub_pid, _sub_name.c_str());
        }

    private:
        int _wfd;              // 写管道文件描述符
        pid_t _sub_pid;        // 子进程pid
        std::string _sub_name; // 子进程名字
    };

public:
    ProcessPool() {
        srand((unsigned int)time(nullptr) ^ (unsigned int)getpid()); // 设置随机数种子
    }
    ~ProcessPool(){}

    void Init(cb_t cb) {
        CreatProcessChannel(cb);
    }

    void Debug() {
        for (auto& ch : channels) {
            ch.PrintInfo();
        }
    }

    void Run() {
        while (1) {
            // 1. 选择一个channel(管道+子进程)，本质是选择一个下标
            int index = SelectChannel();
            std::cout << "Selected channel index: " << index << std::endl;
            sleep(1);
            // 2. 选择一个任务
            int itask = SelectTask();
            std::cout << "Selected task index: " << itask << std::endl;
            sleep(1);
            // 3. 发送任务给指定的channel(管道+子进程)
        }
    }
private:

    int SelectChannel() {
        // 这里简单的轮询选择一个channel
        static int index = 0;
        int selected = index % channels.size();
        ++index;
        return selected;
    }

    int SelectTask() {
        // 轮询选择一个任务
        int itask = rand() % (sizeof(tasks) / sizeof(task_t));
        return itask;
    }

    void CreatProcessChannel(cb_t cb) {
        for (int i = 0; i < POLL_SIZE; ++i) {
            int pipefd[2] = {0};
            int ret = pipe(pipefd);
            if (ret == -1) {
                std::cerr << "pipe error" << std::endl;
                exit(PIPE_ERROR);
            }

            pid_t pid = fork();
            if (pid == -1) {
                std::cerr << "fork error" << std::endl;
                exit(FORK_ERROR);
            }
            else if (pid == 0) {
                // 子进程
                close(pipefd[1]);   // 关闭写端
                cb(pipefd[0]);
                exit(OK);
            }
            else {
                // 父进程
                close(pipefd[0]);   // 关闭读端
                // 创建一个channel对象，保存管道写端和子进程pid
                channels.emplace_back(pipefd[1], pid);
                // Channel ch(pipefd[1], pid);
                // channels.emplace_back(ch);  // 将channel对象添加到容器中
                std::cout << "Created channel for child process with PID: " << pid << std::endl;
                sleep(1);
            }
        }
    }




private:
    std::vector<Channel> channels;    // 要有未来组织所有channel的容器
};
int main() {

    // 1. 初始化一个进程池对象
    ProcessPool pool;
    pool.Init(Dotask);

    // 2. 运行进程池
    pool.Run();




    sleep(100); // 让父进程等待一段时间，观察子进程的运行情况


    return 0;
}