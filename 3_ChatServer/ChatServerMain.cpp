#include <memory>
#include "UdpServer.hpp"   // 获取事件
#include "ThreadPool.hpp"   // 执行者
#include "Route.hpp"        // 任务


using task_t = std::function<void ()>;

static void Usage(const std::string &process) {
    std::cerr << "Usage:\n\t";
    std::cerr << process << " [local_port]" << std::endl;
}

using namespace NS_THREAD_POOL_MODULE;

// ./server_udp [ip] [port]
int main(int argc, char* argv[]) {

    if (argc != 2) {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY()
    uint16_t socket_port = std::stoi(argv[1]);

    // 线程池模块
    auto thread_pool = ThreadPool<task_t>::Instance();

    // 路由模块
    Route rt;

    // 网络服务模块
    UdpServer usvr(socket_port);
    usvr.RegisterService(
        [&rt](const InetAddr& addr){
            rt.CheckUser(addr);
        },
        [&rt, thread_pool](int sockfd, std::string msg){
            auto t = std::bind(&Route::Broadcast, &rt, sockfd, msg);
            thread_pool->Enqueue(t);

            // 等同于
            // thread_pool->Enqueue([&rt, &sockfd, &msg](){
            //     rt.Broadcast(sockfd, msg);
            // });
        });
    usvr.Loop();
    
    return 0;
}