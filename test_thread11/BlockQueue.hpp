#pragma once
#include <iostream>
#include <pthread.h>
#include <queue>
#include "mutex.hpp"
#include "Cond.hpp"

const int defaultcap = 5;

template<typename T>
class BlockQueue
{
private:
    bool IsEmpty() {
        return _bq.empty();
    }
    bool IsFull() {
        return _bq.size() == _cap;
    }

public:
    BlockQueue(int cap = defaultcap):_cap(cap)
    {
        // pthread_mutex_init(&_mutex, nullptr);
        // pthread_cond_init(&_consumer_cond, nullptr);
        // pthread_cond_init(&_productor_cond, nullptr);

        _sleep_comsumer_num = 0;
        _sleep_productor_num = 0;

    }
    ~BlockQueue()
    {
        // pthread_mutex_destroy(&_mutex);
        // pthread_cond_destroy(&_consumer_cond);
        // pthread_cond_destroy(&_productor_cond);
    }


    // 生产者存入
    void Enqueue(T& in) {
        {
            LockGuard lg(_mutex);
            while (IsFull()) {
                ++_sleep_productor_num;
                _productor_cond.Wait(_mutex);
                --_sleep_productor_num;
            }

            _bq.emplace(in);
            if (_sleep_comsumer_num > 0) {
                _consumer_cond.Signal();
            }        
        }
    }


    // 消费者取出
    void Pop(T* out) {
        {
            LockGuard lg(_mutex);
            while (IsEmpty()) {
                ++_sleep_comsumer_num;
                _consumer_cond.Wait(_mutex);
                --_sleep_comsumer_num;
            }
            *out = _bq.front();
            _bq.pop();

            if (_sleep_productor_num > 0) {
                _productor_cond.Signal();
            }
        }
    }
private:
    std::queue<T> _bq;
    int _cap;

    Mutex _mutex;
    Cond _consumer_cond;
    Cond _productor_cond;
    
    // pthread_mutex_t _mutex;
    // pthread_cond_t _consumer_cond;
    // pthread_cond_t _productor_cond;

    // 线程休眠个数
    int _sleep_comsumer_num;
    int _sleep_productor_num;
};