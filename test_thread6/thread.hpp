#pragma
#include <iostream>
#include <pthread.h>
#include <string>
#include <string.h>
#include <functional>

namespace my
{
    static int g_number = 1;
    using callback_t = std::function<void ()>;

    enum class TSTATUS {
        THREAD_NEW,
        THREAD_RUNNING,
        THREAD_STOP
    };

    std::string StatusToString(TSTATUS s) {
        switch (s) {
        case TSTATUS::THREAD_NEW:
            return "THREAD_NEW";
        case TSTATUS::THREAD_RUNNING:
            return "THREAD_RUNNING";
        case TSTATUS::THREAD_STOP:
            return "THREAD_STOP";
        default:
            return "UNKNOWN";
        }
    }

    std::string IsJoin(bool _joinable) {
        return _joinable ? "true" : "false";
    }

    class Thread
    {
    public:
        void ToRunning() {
            _status = TSTATUS::THREAD_RUNNING;
        }

        void ToStop() {
            _status = TSTATUS::THREAD_STOP;
        }

        static void* ThreadRoutine(void* args) {
            Thread* self = static_cast<Thread*>(args);
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_cb();
            self->ToStop();
            return nullptr;
        }

    public:
        Thread(callback_t cb): _tid(-1), _status(TSTATUS::THREAD_NEW), _joinable(true), _cb(cb), _result(nullptr) {
            _name = "New-Thread-" + std::to_string(g_number++);
        }
        ~Thread() {}

        bool Start() {
            int ret = pthread_create(&_tid, nullptr, ThreadRoutine, this);
            if (ret != 0) {
                return false;
            }
            ToRunning();
            return true;
        }

        void Join() {
            if (_joinable) {
                int ret = pthread_join(_tid, &_result);
                if (ret != 0) {
                    std::cerr << "pthread_join失败:" << strerror(ret) << std::endl;
                    return;
                }
                _status = TSTATUS::THREAD_STOP;
            }
            else {
                std::cerr << "不是可汇线程!  join_status:" << IsJoin(_joinable) << std::endl;
            }
        }

        void Stop() {
            if (_status == TSTATUS::THREAD_RUNNING) {
                pthread_cancel(_tid);
                _status = TSTATUS::THREAD_STOP;
            }
        }

        void Detach() {
            if (_status == TSTATUS::THREAD_RUNNING && _joinable) {
                pthread_detach(_tid);
                _joinable = false;
            }
            else {
                std::cerr << "detach" << _name << "失败" << std::endl;
            }
        }

        void PrintInfo() {
            std::cout << "thread name: " << _name << std::endl;
            std::cout << "thread tid: " << _tid << std::endl;
            std::cout << "thread status: " << StatusToString(_status) << std::endl;
            std::cout << "thread joinable: " << IsJoin(_joinable) << std::endl;
        }
    private:
        std::string _name;
        pthread_t _tid;
        TSTATUS _status;
        bool _joinable;
        callback_t _cb;
        void* _result;
    };
}
