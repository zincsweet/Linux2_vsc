#pragma once
#include <iostream>

class Command
{
public:
    std::string Execute(const std::string &cmd) {
        std::cout << "In Command..." << std::endl;
        return "Command";
    }
};