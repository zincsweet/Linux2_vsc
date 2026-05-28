#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


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

    if (pid == 0) {
        // Child process
        close(fd[0]); // Close the read end of the pipe

        const char *message = "Hello from the child process!";
        write(fd[1], message, strlen(message) + 1); // Write message to the pipe

        close(fd[1]); // Close the write end of the pipe
    } else {
        // Parent process
        close(fd[1]); // Close the write end of the pipe

        wait(NULL); // Wait for the child process to finish

        read(fd[0], buffer, sizeof(buffer)); // Read message from the pipe
        printf("Received message: %s\n", buffer);

        close(fd[0]); // Close the read end of the pipe
    }
}