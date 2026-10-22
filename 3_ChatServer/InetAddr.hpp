#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// 对客户端进行先描述
class InetAddr
{
public:
    InetAddr(const struct sockaddr_in &address)
    :_address(address),
     _len(sizeof(address))
    {
        _port = ntohs(_address.sin_port);
        char _ip_char[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &_address.sin_addr, _ip_char, sizeof(_ip_char));
        _ip.assign(_ip_char);
    }
    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0")
    :_ip(ip),
     _port(port)
    {
        memset(&_address, 0, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_port = htons(_port);
        inet_pton(AF_INET, _ip.c_str(), &_address.sin_addr);
        _len = sizeof(_address);
    }
    InetAddr()
    {}
    ~InetAddr()
    {}

    bool operator==(const InetAddr &addr) {
        return (this->_ip == addr._ip) && (this->_port == addr._port);
    }

    struct sockaddr_in* GetNetAddress() {
        return &_address;
    }

    socklen_t Len() {
        return _len;
    }

    std::string ToString() {
        return _ip + ":" + std::to_string(_port);
    }

private:
    // net地址
    struct sockaddr_in _address;
    socklen_t _len;

    // host地址
    std::string _ip;
    uint16_t _port;
};