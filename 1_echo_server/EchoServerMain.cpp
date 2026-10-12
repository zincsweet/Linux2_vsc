#include "EchoServer.hpp"
#include <memory>

static void Usage(const std::string &process) {
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_ip local_port" << std::endl;
}

// ./server_udp [ip] [port]
int main(int argc, char* argv[]) {

    if (argc != 3) {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY()

    std::string socket_ip = argv[1];
    uint16_t socket_port = std::stoi(argv[2]);

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(socket_ip, socket_port);
    usvr->Init();
    usvr->Loop();
    return 0;
}