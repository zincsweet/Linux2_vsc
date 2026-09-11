#include "Logger.hpp"

using namespace NS_LOG_MODULE;

int main() {
    // std::cout << GetCurrentTime() << std::endl;
    // sleep(1);
    // std::cout << GetCurrentTime() << std::endl;
    // sleep(1);
    // std::cout << GetCurrentTime() << std::endl;
    // sleep(1);
    // std::cout << GetCurrentTime() << std::endl;
    // sleep(1);

    // ENABLE_CONSOLE_LOG_STRATEGY();
    // logger.Debug("console strategy!");
    // logger.Debug("console strategy!");
    // logger.Debug("console strategy!");

    // ENABLE_FILE_LOG_STRATEGY();
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");

    ENABLE_FILE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) << "Hello World" << "1234";
    LOG(LogLevel::ERROR) << "Hello World" << "1234";

    return 0;
}