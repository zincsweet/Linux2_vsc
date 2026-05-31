#pragma once
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdio>

// 用户指明
#define PATHNAME "."
#define PROJ_ID 0x66

const int gsize = 4096;

class Shm {
public:
    Shm()
        : _shmid(-1)
        , _size(gsize)
        , _start_addr(nullptr)
    {}

    ~Shm() {}



    void Delete() {
        int n = shmctl(_shmid, IPC_RMID, nullptr);
        if (n < 0) {
            std::cerr << "shmctl 删除失败:" << strerror(errno) << std::endl;
            exit(5);
        }
        std::cout << "删除成功！" << std::endl;
    }

    void Attach() {
        _start_addr = shmat(_shmid, nullptr, 0);
        if (_start_addr == (void*)-1) {
            std::cerr << "shmat挂接失败:" << strerror(errno) << std::endl;
            exit(3);
        }
    }

    void Detach() {
        int n = shmdt(_start_addr);
        if (n == -1) {
            std::cerr << "shmdt解除挂接失败:" << strerror(errno) << std::endl;
            exit(4);
        }
        std::cout << "解除挂接成功" << std::endl;
    }

    void Get() {
        GetHelper(IPC_CREAT);
    }

    void Create() {
        GetHelper(IPC_CREAT | IPC_EXCL | 0666);
    }

    void* Addr() {
        return _start_addr;
    }

    int Size() {
        return _size;
    }

private:
    key_t Getkey() {
        return ftok(PATHNAME, PROJ_ID);
    }

    void GetHelper(int shmflg) {
        // 1. 构建键值
        key_t k = Getkey();
        if (k < 0) {
            std::cerr << "获取key键值失败:" << strerror(errno) << std::endl;
            exit(1);
        }

        // 2. 创建共享内存
        _shmid = shmget(k, _size, shmflg);
        if (_shmid < 0) {
            std::cerr << "shmget创建共享内存失败:" << strerror(errno) << std::endl;
            exit(2);
        }
        printf("16进制key:0x%x; _shmid:%d\n", k, _shmid);
    }

private:
    int _shmid;
    int _size;
    void* _start_addr;
};