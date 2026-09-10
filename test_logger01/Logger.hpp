#pragma once
#include <iostream>
#include <string>
#include <sys/time.h>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include "mutex.hpp"


namespace NS_LOG_MODULE
{

    // 日志等级信息
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    // 为避免打印日志时，流式输出等级是数字
    std::string LogLevelToMessage(LogLevel level) {
        switch (level)
        {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOW";
        }
    }


    // 详细的时间信息
    std::string GetCurrentTime() {
        struct timeval current_time;
        gettimeofday(&current_time, nullptr);
        // current_time.tv_sec; current_time.tv_usec;

        struct tm struct_time;
        localtime_r(&(current_time.tv_sec), &struct_time);
        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d.%ld",
            struct_time.tm_year + 1900,
            struct_time.tm_mon + 1,
            struct_time.tm_mday,
            struct_time.tm_hour,
            struct_time.tm_min,
            struct_time.tm_sec,
            current_time.tv_usec
        );
        return timestr;
    }


    // 策略模式，策略接口
    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };



    // 显示器打印策略
    class ConsoleStrategy : public LogStrategy
    {
    public:
        ~ConsoleStrategy()
        {}
        void SyncLog(const std::string &message) override
        {
            LockGuard lg(_mutex);
            std::cout << message << std::endl;
        }

    private:
        Mutex _mutex;
    };


    // 文件打印策略
    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";
    class FileStrategy : public LogStrategy
    {
    public:
        FileStrategy(const std::string &path = defaultpath, const std::string &filename = defaultfilename)
        :_logpath(path)
        ,_logfilename(filename)
        {
            LockGuard lg(_mutex);
            if (std::filesystem::exists(_logpath)) return;
            try
            {
                std::filesystem::create_directories(_logpath);
            }
            catch (std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << std::endl;
            }
            if (!_logpath.empty() && _logpath.back() != '/') _logpath += "/";
        }
        ~FileStrategy()
        {}
        void SyncLog(const std::string &message) override
        {
            LockGuard lg(_mutex);
            std::string targetlog = _logpath + _logfilename;    // "./log/log.txt"
            std::ofstream out(targetlog, std::ios::app);        // 追加的方式写入
            if (!out.is_open())
            {
                std::cerr << "open " << targetlog << " failed!" << std::endl;
                return;
            }
            out << message << '\n';
        }
    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };



    class Logger
    {
        // 日志类：
        // 1. 日志的生成
        // 2. 根据不同的策略进行刷新
    public:
        Logger()
        {}
        ~Logger()
        {}

        void UseConsoleStrategy() {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
        void UseFileStrategy() {
            _strategy = std::make_unique<FileStrategy>();
        }
        // void Debug(const std::string &message) {
        //     if (_strategy != nullptr) {
        //         _strategy->SyncLog(message);
        //     }
        // }


        // 内部类
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string& filename, int line, Logger& logger)
            :_curr_time(GetCurrentTime()),
             _level(level),
             _pid(getpid()),
             _filename(filename),
             _line(line),
             _logger(logger)
            {
                // 一条完整的日志信息 = 前部分固定信息 + 后面不固定信息

                // 先组成前部分固定
                std::stringstream ss;
                ss << '[' << _curr_time << "] "
                   << '[' << LogLevelToMessage(_level) << "] "
                   << '[' << _pid << "] "
                   << '[' << _filename << "] "
                   << '[' << _line << "] "
                   << " - ";
                
                _loginfo = ss.str();
            }
            ~LogMessage()
            {
                // 在Logger析构时，进行刷新信息（RAII）
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

            template<typename T>
            LogMessage& operator << (const T& info) {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;   // 返回当前LogMessage，以便连续<<
            }


        private:
            std::string _curr_time; // 时间
            LogLevel _level;        // 等级
            pid_t _pid;             // 进程id
            std::string _filename;  // 文件名
            int _line;              // 具体位置

            std::string _loginfo;   // 一条完整的日志信息

            // 因为这是内部类LogMessage，析构时，要知道Logger的刷新策略，所以要引用外面的Logger
            Logger& _logger;
        };


        // 不是内部类了
        // 重载的是Logger类的(),只是返回类型是LogMessage
        LogMessage operator()(LogLevel level, std::string filename, int line) {
            return LogMessage(level, filename, line, *this);
        }
        // 意义：创建出那个用来实现 RAII 的临时 LogMessage 对象

    private:
        std::unique_ptr<LogStrategy> _strategy;
    };


    // 日志对象，全局使用
    Logger logger;

#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy();

}