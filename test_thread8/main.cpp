#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mutex.hpp"

int ticket = 1000;
Mutex lock;

void *route(void *arg)
{
    char *id = (char*)arg;
    while (true) {
        // 临界区
        {
            LockGuard guard(lock);
            // lock.Lock();
            if ( ticket > 0 ) {
                usleep(1000);
                printf("%s sells ticket:%d\n", id, ticket);
                ticket--;
                // lock.UnLock();
            } else {
                // lock.UnLock();
                break;
            }
        }
    }
    return nullptr;
}

int main( void )
{
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, route, (void*)"thread 1");
    pthread_create(&t2, NULL, route, (void*)"thread 2");
    pthread_create(&t3, NULL, route, (void*)"thread 3");
    pthread_create(&t4, NULL, route, (void*)"thread 4");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
}