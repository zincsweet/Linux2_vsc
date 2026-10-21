#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#include "Logger.hpp"

using namespace NS_LOG_MODULE;

static const std::string default_path = "./dict.txt";
static const std::string sep = ": ";

class Dict
{
public:
    Dict(const std::string &dict_path = default_path)
    :_dict_path(dict_path)
    {
        LoadDict();
    }
    ~Dict()
    {}

    void LoadDict() {
        std::ifstream in(_dict_path);
        if (!in.is_open()) {
            LOG(LogLevel::FATAL) << "open [" << _dict_path << "] error";
            exit(1);
        }

        std::string line;
        while (std::getline(in, line)) {
            LOG(LogLevel::INFO) << "load [" << line << "] success";
            // apple: 苹果
            auto sep_pos = line.find(sep);
            /*
            std::string::npos 精简总结
            本质：static const size_type npos = -1;，无符号回绕成极大值，作"无效位置"标记。
            含义：find 系列找不到时返回它 → "没找到"。
            判断：if (s.find("x") != std::string::npos) 找到；== 没找到。别写成 if (s.find(...))。
            接收：用 auto 或 size_t，别用 int。
            附带用途：substr(pos) 的第二参数默认就是 npos，表示"取到末尾"。
            坑：别把 npos 当下标传给 substr（会抛 out_of_range）。
            */
            if (sep_pos == std::string::npos) {
                LOG(LogLevel::WARNING) << "Format: [" << line << "] error";
                continue;
            }

            std::string k = line.substr(0, sep_pos);
            std::string v = line.substr(sep_pos + sep.size());
            _dict.insert({k, v});
        }

        LOG(LogLevel::INFO) << "load done...";
    }

    std::string Translate(std::string word) {
        auto it = _dict.find(word);
        if (it != _dict.end()) {
            return it->second;
        }
        else {
            return "None";
        }
    }
private:
    std::string _dict_path;
    std::unordered_map<std::string, std::string> _dict;
};