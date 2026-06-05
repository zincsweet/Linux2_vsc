#pragma
#include <string>

class Task {
public:
    Task(int x, int y) 
        : _x(x)
        , _y(y) 
    {}
    ~Task(){}

    void Excute() {
        _result = _x + _y;
    }

    std::string Result() {
        return std::to_string(_x) + "+" + std::to_string(_y) + "=" + std::to_string(_result);
    }
private:
    int _x;
    int _y;
    int _result;
};