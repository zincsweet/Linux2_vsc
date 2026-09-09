#pragma once
#include <string>

// using task_t = std::function<void()>;

// 代表可以包装任意 无参数、返回值 void 的可调用对象（普通函数、lambda、仿函数）
// task_t 就是这个函数类型的别名，后面写代码直接用task_t代替长长的std::function<void()>

// void Print() {
//     std::cout << "I am a task." << std::endl;
// }

class Task
{
public:

    Task()
    {}

    Task(int x, int y)
    :_x(x)
    ,_y(y)
    {}

    ~Task()
    {}

    void Execute() {
        _result = _x + _y;
    }

    void operator()() {
        Execute();
    }

    std::string getResult() {
        return std::to_string(_x) + " + " + std::to_string(_y) + " = " + std::to_string(_result);
    }

    std::string Question() {
        return std::to_string(_x) + " + " + std::to_string(_y) + " = ?";
    }

private:
    int _x;
    int _y;
    int _result;
};