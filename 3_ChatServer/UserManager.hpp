#pragma once

#include <string>
#include <vector>

#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;


class UserManager
{
public:
    UserManager()
    {}
    ~UserManager()
    {}

    void AddUser(const InetAddr &addr) {
        if (SearchUser(addr)) return;
        _users.emplace_back(addr);
    }

    void DelUser(const InetAddr &addr) {
        if (!SearchUser(addr)) {
            LOG(LogLevel::WARNING) <<  "This addr Unexist.";
        }

        for (auto it = _users.begin(); it != _users.end(); ++it) {
            if (*it == addr) {
                _users.erase(it);
                break;
            }
        }
    }

    bool SearchUser(const InetAddr &addr) {
        for (auto& user : _users) {
            if (user == addr) {
                return true;
            }
        }
        return false;
    }

    // bool ModUser(const InetAddr &addr) {

    // }

    std::vector<InetAddr>& Users() {
        return _users;
    }

private:
    std::vector<InetAddr> _users;
};