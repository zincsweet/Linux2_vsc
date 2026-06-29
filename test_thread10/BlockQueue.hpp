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

        // _blockqueue_high_water = _cap * 2 / 3;
        // _blockqueue_low_water = _cap * 1 / 3;
        _sleep_comsumer_num = 0;
        _sleep_productor_num = 0;

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

        while (IsFull()) {
            ++_sleep_productor_num;
            pthread_cond_wait(&_productor_cond, &_mutex);
            --_sleep_productor_num;
        }
        _bq.emplace(in);
        // if (_bq.size() > _blockqueue_high_water) {
        //     pthread_cond_signal(&_consumer_cond);
        // }
        if (_sleep_comsumer_num > 0) {
            pthread_cond_signal(&_consumer_cond);
        }        
        pthread_mutex_unlock(&_mutex);
    }


    // 消费者取出
    void Pop(T* out) {
        pthread_mutex_lock(&_mutex);

        while (IsEmpty()) {
            ++_sleep_comsumer_num;
            pthread_cond_wait(&_consumer_cond, &_mutex);    // 1. 过量的唤醒信息； 2. 函数调用失败； 3. 伪唤醒
            --_sleep_comsumer_num;
        }
        // 100% bq肯定有数据
        *out = _bq.front();
        _bq.pop();
        // if (_bq.size() < _blockqueue_low_water) {
        //     pthread_cond_signal(&_productor_cond);
        // }
        if (_sleep_productor_num > 0) {
            pthread_cond_signal(&_productor_cond);
        }
        pthread_mutex_unlock(&_mutex);
    }
private:
    std::queue<T> _bq;
    int _cap;

    pthread_mutex_t _mutex;
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;

    // 水位线
    // int _blockqueue_low_water;
    // int _blockqueue_high_water;

    // 线程休眠个数
    int _sleep_comsumer_num;
    int _sleep_productor_num;
};