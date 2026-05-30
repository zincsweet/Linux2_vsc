#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <functional>
#include <sys/wait.h>
#include <sys/types.h>

#define POLL_SIZE 4

// ***********************************任务列表*********************************************
void SyncDisk() {
    std::cout << "同步磁盘..." << std::endl;
}

void DownloadFile() {
    std::cout << "下载文件..." << std::endl;
}

void PrintMessage() {
    std::cout << "打印消息..." << std::endl;
}

void UpdateDatabase() {
    std::cout << "更新数据库..." << std::endl;
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
        int task_code = 0;
        ssize_t n = read(fd, &task_code, sizeof(task_code));    // 约定好读取4字节的任务码
        if (n == -1) {
            std::cerr << "read error" << std::endl;
            break;
        }
        else if (n == 0) {
            std::cout << "没有任务了..." << std::endl;
            break; // 管道被关闭，退出循环
        }
        else {
            if (task_code < 0 || task_code >= sizeof(tasks) / sizeof(task_t)) {
                std::cerr << "Invalid task code: " << task_code << std::endl;
                continue; // 跳过无效的任务码
            }
            // 根据任务码执行对应的任务函数
            tasks[task_code]();
        }

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
        void Write(int task_index) {
            ssize_t n = write(_wfd, &task_index, sizeof(task_index));
            (void)n; // 忽略写入结果
        }


        void PrintInfo() {
            printf("Channel Info - Sub PID: %d, Sub Name: %s\n", _sub_pid, _sub_name.c_str());
        }

        std::string GetSubName() const {
            return _sub_name;
        }
        void ClosePipe() {
            std::cout << "关闭管道wfd: " << _wfd << std::endl;
            close(_wfd);
        }

        void Wait() {
            waitpid(_sub_pid, nullptr, 0);
            std::cout << "回收子进程: " << " PID: " << _sub_pid << std::endl;
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
        int cnt = 6;
        while (cnt--) {
            std::cout << "---------------------------------------------" << std::endl;
            // 1. 选择一个任务
            int itask = SelectTask();
            std::cout << "选择任务： index: " << itask << std::endl;

            // 2. 选择一个channel(管道+子进程)，本质是选择一个下标
            int index = SelectChannel();
            std::cout << "选择管道： index: " << index << std::endl;

            // 3. 发送任务给指定的channel(管道+子进程)
            SendTaskToChannel(itask, index);
            std::cout << "发送任务 index " << itask << " 到管道 index " << index << std::endl;
        }
    }

    void Quit() {

        // 想要1 : 1地回收，就要改变CreateProcessChannel的实现
        // 就要在子进程中关闭历史遗留的管道写端
        // 从而实现1 : 1地回收资源。
        for (auto& ch : channels) {
            ch.ClosePipe(); // 关闭写端
            ch.Wait();
        }

        // 逆向回收资源
        // int end = channels.size() - 1;
        // for (int i = end; i >= 0; --i) {
        //     channels[i].ClosePipe(); // 关闭写端
        //     channels[i].Wait();      // 等待子进程退出
        // }

        // 1:1回收演示
        // for (auto& ch : channels) {
        //     ch.ClosePipe(); // 关闭写端
        //     ch.Wait();
        // }

        // // 1. 关闭所有管道写端，通知子进程退出
        // for (auto& ch : channels) {
        //     ch.ClosePipe(); // 关闭写端
        // }
        // // 2. 等待所有子进程退出
        // for (auto& ch : channels) {
        //     ch.Wait();
        // }
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

    void SendTaskToChannel(int itask, int index) {
        if (itask < 0 || itask >= sizeof(tasks) / sizeof(task_t)) {
            std::cerr << "Invalid task index: " << itask << std::endl;
            return;
        }
        if (index < 0 || index >= channels.size()) {
            std::cerr << "Invalid channel index: " << index << std::endl;
            return;
        }
        // 将任务索引写入管道，通知子进程执行对应的任务
        channels[index].Write(itask);
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
                if (!channels.empty()) {
                    for (auto& ch : channels) {
                        ch.ClosePipe(); // 关闭历史遗留的管道写端
                    }
                }
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
                std::cout << "创建了一个管道PID: " << pid << "到管道容器中了" << std::endl;
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

    // 3. 结束进程池释放资源
    pool.Quit();




    return 0;
}