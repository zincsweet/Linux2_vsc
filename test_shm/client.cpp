#include "shm.hpp"

int main() {
    // 不需要创建内核级共享内存，当然也不需要删除
    Shm sharedmem;
    sharedmem.Get();
    sharedmem.Attach();
    
    char* shm_start = (char*)sharedmem.Addr();
    int size = sharedmem.Size();
    int index = 0;
    while (true) {
        std::cout << "请输入@ ";
        char ch;
        std::cin >> ch;
        shm_start[index++] = ch;
        index %= size;
    }

    sharedmem.Detach();
    return 0;
}