#include <iostream>
#include <string>
#include <signal.h>
#include <cstring>


void Usage(const std::string& cmd) {
    std::cout << "Usage:" << cmd << " signo pid" << std::endl;
}

// ./mykill signo pid
int main(int argc, char* argv[]) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }
    int signo = std::stoi(argv[1]);
    pid_t pid = std::stoi(argv[2]);

    int r = kill(pid, signo);
    if (r == -1) {
        std::cerr << "kill失败:" << strerror(errno) << std::endl;
        exit(2);
    }

    return 0;
}