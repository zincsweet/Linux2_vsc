#pragma once
#include <semaphore.h>
#include <iostream>

class Sem
{
public:
    Sem(int int_val)
    {
        if (int_val >= 0) {
            sem_init(&_sem, 0, int_val);
        }
    }
    ~Sem()
    {
        sem_destroy(&_sem);
    }

    void P() {
        sem_wait(&_sem);
    }
    void V() {
        sem_post(&_sem);
    }
private:
    sem_t _sem;
};