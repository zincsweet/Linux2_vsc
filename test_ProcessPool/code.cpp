#include <iostream>
#include <string>
#include <unistd.h>

#define POLL_SIZE 4

enum {
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};

void Dotask() {

}


// 父进程通过管道向子进程发送数据，子进程通过管道接收数据
class Channel {
public:
    Channel()
    {}
    ~Channel()
    {}

private:
    int _wfd;   //写管道文件描述符
    pid_t _sub_pid; //子进程pid
    std::string _sub_name; //子进程名字
};

int main() {

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
            // 关闭写端
            close(pipefd[1]);
            Dotask();
            exit(OK);
        }
        else {}
        // 父进程
        // 关闭读端
        close(pipefd[0]);
    }

    return 0;
}