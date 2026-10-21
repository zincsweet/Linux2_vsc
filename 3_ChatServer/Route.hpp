#pragma once

#include <string>
#include <memory>
#include "mutex.hpp"
#include "UserManager.hpp"

class Route
{
public:
    Route()
    :_uma(std::make_unique<UserManager>())
    {}
    ~Route()
    {}

    void Broadcast(int sockfd, std::string message) {
        LockGuard lg(_lock);
        auto& users = _uma->Users();
        for (auto& user : users) {
            sendto(sockfd, message.c_str(), sizeof(message), 0, (struct sockaddr*)user.GetNetAddress(), user.Len());
        }
    }

    void CheckUser(const InetAddr& addr) {
        LockGuard lg(_lock);
        _uma->AddUser(addr);
    }

    void OffLineUser(const InetAddr& addr) {
        LockGuard lg(_lock);
        _uma->DelUser(addr);
    }

private:
    std::unique_ptr<UserManager> _uma;
    Mutex _lock;
};