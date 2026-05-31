#include "shm.hpp"

int main() {
    // 谁创建，谁删除
    Shm sharedmem;
    sharedmem.Create();
    sharedmem.Attach();

    char* shm_start = (char*)sharedmem.Addr();
    int size = sharedmem.Size();
    while (true) {
        for (int i = 0; i <size; ++i) {
            std::cout << shm_start[i] << ' ';
        }
        std::cout << std:: endl;
        sleep(3);
    }

    sharedmem.Detach();
    sharedmem.Delete();
    return 0;
}