#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "mutex.hpp"
#include "Logger.hpp"
#include "thread.hpp"
#include "Cond.hpp"

namespace NS_THREAD_POOL_MODULE
{
    using namespace NS_THREAD_MODULE;
    using namespace NS_LOG_MODULE;

    const int defaultnum = 5;



    // 要对线程池进行管理——先描述再组织！
    template<typename T>
    class ThreadPool
    {
    private:
        void HandlerTask() {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (true) {
                // 保护临界区
                _mutex.Lock();
                // 检测有无任务
                while (_isrunning && _tasks.empty()) {
                    // 没有任务，休眠
                    ++_slaver_sleep_count;
                    _cond.Wait(_mutex);
                    --_slaver_sleep_count;
                }

                // 若果是线程池要结束运行，正好没任务
                if (!_isrunning && _tasks.empty()) {
                    _mutex.UnLock();
                    break;
                }

                // 有任务，取任务(本质:将任务由公共的变为私有)
                T task = _tasks.front();
                _tasks.pop();
                _mutex.UnLock();

                // 处理任务不需要在临界区内部处理
                LOG(LogLevel::INFO) << name << "处理任务：";
                task();
            }
        }

    public:
        ThreadPool(int slaver_num = defaultnum)
        :_isrunning(false),
         _slaver_num(slaver_num),
         _slaver_sleep_count(0)
        {
            
            for (int i = 0; i < _slaver_num; ++i) {

                // auto f = std::bind(&ThreadPool::HandlerTask, this);
                // _slavers.emplace_back(f);

                _slavers.emplace_back([this](){
                    this->HandlerTask();
                });
            }
        }
        ~ThreadPool()
        {}
        void Start() {
            if (_isrunning) {
                LOG(LogLevel::WARNING) << "Thread Pool Is Already Running!";
                return;
            }
            _isrunning = true;
            for (auto& slave : _slavers) {
                slave.Start();
            }
        }

        void Stop() {
            // -----------------------------------
            // if (!_isrunning) {
            //     LOG(LogLevel::WARNING) << "Thread Is Not Running!";
            //     return;
            // }
            // for (auto& slave : _slavers) {
            //     slave.Stop();
            // }
            // _isrunning = false;
            // -----------------------------------

            // 1. _isrunning = faulse
            // 2. _tasks内所有任务都要处理完
            _mutex.Lock();
            _isrunning = false;
            if (_slaver_sleep_count > 0) {
                _cond.Broadcast();
            }
            _mutex.Lock();
        }

        void Wait() {
            for (auto& slave : _slavers) {
                slave.Join();
            }
        }

        void Enqueue(T in) {
            _mutex.Lock();
            _tasks.push(in);
            if (_slaver_sleep_count > 0) {
                _cond.Signal();
            }
            _mutex.UnLock();
        }
    private:
        bool _isrunning;
        int _slaver_num;
        std::vector<Thread> _slavers;
        std::queue<T> _tasks;   // 任务队列，临界资源
        Mutex _mutex;
        Cond _cond;
        int _slaver_sleep_count;
    };
}