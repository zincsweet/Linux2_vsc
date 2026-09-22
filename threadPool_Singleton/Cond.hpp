#pragma once

#include "mutex.hpp"
#include <pthread.h>

class Cond
{
public:
    Cond()
    {
        pthread_cond_init(&_cond,nullptr);
    }
    ~Cond()
    {
        pthread_cond_destroy(&_cond);
    }

    void Wait(Mutex &mutex) {
        pthread_cond_wait(&_cond,mutex.Ptr());
    }
    void Signal() {
        pthread_cond_signal(&_cond);
    }
    void Broadcast() {
        pthread_cond_broadcast(&_cond);
    }
private:
    pthread_cond_t _cond;
};