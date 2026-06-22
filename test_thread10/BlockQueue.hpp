#pragma once
#include <iostream>
#include <pthread.h>
#include <queue>

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
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer_cond, nullptr);
        pthread_cond_init(&_productor_cond, nullptr);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer_cond);
        pthread_cond_destroy(&_productor_cond);
    }


    // 生产者存入
    void Enqueue(T& in) {
        pthread_mutex_lock(&_mutex);

        if (IsFull()) {
            pthread_cond_wait(&_productor_cond, &_mutex);
        }
        _bq.emplace(in);
        pthread_cond_signal(&_consumer_cond);

        pthread_mutex_unlock(&_mutex);
    }


    // 消费者取出
    void Pop(T* out) {
        pthread_mutex_lock(&_mutex);

        if (IsEmpty()) {
            pthread_cond_wait(&_consumer_cond, &_mutex);
        }
        *out = _bq.front();
        _bq.pop();
        pthread_cond_signal(&_productor_cond);

        pthread_mutex_unlock(&_mutex);
    }
private:
    std::queue<T> _bq;
    int _cap;

    pthread_mutex_t _mutex;
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;

    // int _blockqueue_low_water;
    // int _blockqueue_high_water;
};