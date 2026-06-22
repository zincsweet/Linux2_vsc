#pragma
#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex() {
        pthread_mutex_init(&_lock, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&_lock);
    }
    void Lock() {
        pthread_mutex_lock(&_lock);
    }
    void UnLock() {
        pthread_mutex_unlock(&_lock);
    }
private:
    pthread_mutex_t _lock;
};

// 采用RAII风格,进行锁管理
class LockGuard
{
public:
    LockGuard(Mutex& lock):_lockref(lock) {
        _lockref.Lock();
    }
    ~LockGuard() {
        _lockref.UnLock();
    }
private:
    Mutex& _lockref;
};