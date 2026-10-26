#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include "InetAddr.hpp"

static const int default_sockfd = -1;
static const int default_retry_interval = 1;
static const int default_max_retries = 5;


enum ExitCode
{
    SUCCESS,
    USAGE_ERR,
    SOCKET_ERR
};

enum class Status
{
    NEW,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,   // 失联
    CLOSED          // 重连失败
};


class ClientConnection
{
public:
    ClientConnection(const std::string &server_ip, uint16_t server_port)
    :_sockfd(default_sockfd),
     _server_ip(server_ip),
     _server_port(server_port),
     _status(Status::NEW),
     _retry_interval(default_retry_interval),
     _max_retries(default_max_retries)
    {}
    ~ClientConnection()
    {}

    Status ConnectionStatus() {
        return _status;
    }

    void Connect() {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0) {
            std::cerr << "客户端socket()创建套接字失败。" << std::endl;
            exit(ExitCode::SOCKET_ERR);
        }
        InetAddr server_address(_server_port, _server_ip);
        int n = connect(_sockfd, (struct sockaddr*)server_address.GetNetAddress(), server_address.Len());
        if (n < 0) {
            // 1. 断开连接（便捷、彻底）
            DisConnect();
            // 2. 更新状态
            _status = Status::DISCONNECTED;
            // 3. 返回
            return;
        }
        _status = Status::CONNECTED;
        std::cout << "连接服务端成功！" << std::endl;
    }

    void ReConnect() {
        _status = Status::CONNECTING;
        int cnt = 0;
        while (true) {
            Connect();
            if (_status == Status::CONNECTED) {
                break;
            }
            ++cnt;
            if (cnt > _max_retries) {
                _status = Status::CLOSED;
                std::cout << "重连失败，请检查你的网络..." << std::endl;
                break;
            }
            std::cout << "断线重连中，重连次数：" << cnt << std::endl;
            sleep(_retry_interval);
        }
    }

    void Process() {    // 正常的IO通信
        while (true) {
            std::string line;
            std::cout << "请输入需要发送的消息# " ;
            std::getline(std::cin, line);
            ssize_t n = write(_sockfd, line.c_str(), line.size());
            if (n > 0) {
                char inbuffer[1024];
                int r = read(_sockfd, inbuffer, sizeof(inbuffer) - 1);
                if(r > 0) {
                    inbuffer[r] = 0;
                    std::cout << inbuffer << std::endl;
                }
                else if (r == 0) {
                    std::cout << "未读到数据或已经读到文件末尾" << std::endl;
                    _status = Status::DISCONNECTED;
                    break;
                }
                else {
                    std::cerr << "read()读取错误" << std::endl;
                    _status = Status::DISCONNECTED;
                    break;
                }
            }
            else {
                std::cerr << "write error---" << strerror(errno) << std::endl;
                _status = Status::CLOSED;
                break;
            }
        }
    }

    void DisConnect() {
        if (_sockfd > default_sockfd) {
            close(_sockfd);
            _sockfd = default_sockfd;
            _status = Status::CLOSED;
        }
    }
private:
    int _sockfd;
    std::string _server_ip;
    uint16_t _server_port;
    Status _status;
    int _retry_interval;    // 重试的时间间隔
    int _max_retries;       // 最大的重试次数
};

class TcpClient
{
public:
    TcpClient(const std::string &server_ip, uint16_t server_port)
    :_connection(server_ip, server_port)
    {}
    ~TcpClient()
    {}

    void Execute() {
        while (true) {
            switch (_connection.ConnectionStatus()) {
                case Status::NEW:
                    _connection.Connect();
                    break;
                case Status::CONNECTED:
                    _connection.Process();
                    break;
                case Status::DISCONNECTED:
                    _connection.ReConnect();
                    break;
                case Status::CLOSED:
                    _connection.DisConnect();
                    return;
                default:
                    break;
            }
        }
    }
private:
    ClientConnection _connection;
};


void Usage(const std::string &process) {
    std::cout << "Usage:\n\t";
    std::cout << process << "[server_ip] [server_port]" << std::endl;
}

// ./client_tcp [server_ip] [server_port]
int main(int argc, char* argv[]) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(ExitCode::USAGE_ERR);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    TcpClient tc(server_ip, server_port);
    tc.Execute();
    return 0;
}