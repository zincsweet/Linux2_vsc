#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd[2];
    char buffer[1024];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        close(fd[0]);
        sleep(1);

        write(fd[1], "child_write", 11);
        exit(0);
    } else { // Parent process
        close(fd[0]);

        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WIFEXITED(wstatus)) {
            printf("子进程正常退出\n");
        } 
        else if (WIFSIGNALED(wstatus)) { // 被信号杀死
            printf("子进程被信号杀死！信号编号：%d (SIGPIPE)\n", WTERMSIG(wstatus));
        }

    }
    return 0;
}