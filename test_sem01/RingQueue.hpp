#pragma once
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>
#include "Sem.hpp"
#include "mutex.hpp"

const int defalt_cap = 5;

template<typename T>
class RingQueue
{
public:
    RingQueue(int cap = defalt_cap)
    :_cap(cap),
     _rp(cap),
     _consumer_step(0),
     _productor_step(0),
     _blank_sem(cap),
     _data_sem(0)
    {}
    ~RingQueue()
    {}

    void Enqueue(T& in) {   // 生产者调用进行生产数据资源
        // 1. 预定空格资源
        _blank_sem.P();

        {
            LockGuard lg(_pmutex)
            // 2. 找位置生产
            _rp[_productor_step++] = in;
            _productor_step %= _cap;
        }
        // 3. 给出数据资源
        _data_sem.V();
    }

    void Pop(T* out) {      // 消费者调用进行消费数据资源
        // 1. 预定数据资源
        _data_sem.P();

        {
            LockGuard lg(_cmutex);
            // 2. 把数据弹出
            *out = _rp[_consumer_step++];
            _consumer_step %= _cap;
        }
        // 3. 给出空白资源
        _blank_sem.V();
    }

private:
    int _cap;           // 环形队列的容量
    std::vector<T> _rp; // 环形队列

    int _consumer_step; // 消费位置
    int _productor_step;// 生产位置

    Sem _blank_sem;     // 空格资源，生产者关心
    Sem _data_sem;      // 数据资源，消费者关心

    Mutex _cmutex;      // 消费者之间的锁
    Mutex _pmutex;      // 生产者之间的锁
};