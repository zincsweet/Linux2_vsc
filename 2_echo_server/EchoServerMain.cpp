#include "EchoServer.hpp"
#include "Dict.hpp"
#include <memory>

static void Usage(const std::string &process) {
    std::cerr << "Usage:\n\t";
    std::cerr << process << " [local_port]" << std::endl;
}

// ./server_udp [ip] [port]
int main(int argc, char* argv[]) {

    if (argc != 2) {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY()

    // 1. 定义字典
    Dict dict;

    // 2. 构建网络服务，处理IO问题
    // std::string socket_ip = argv[1];
    uint16_t socket_port = std::stoi(argv[1]);

    // 3. 绑定上下两层
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>([&dict](std::string word){
        return dict.Translate(word);
    },socket_port);
    usvr->Init();
    usvr->Loop();
    return 0;
}