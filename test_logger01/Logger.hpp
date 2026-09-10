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
#include "mutex.hpp"


namespace NS_LOG_MODULE
{
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

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
        void Debug(const std::string &message) {
            if (_strategy != nullptr) {
                _strategy->SyncLog(message);
            }
        }
    private:
        std::unique_ptr<LogStrategy> _strategy;
    };

    
    // 日志对象，全局使用
    Logger logger;

#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy();

}