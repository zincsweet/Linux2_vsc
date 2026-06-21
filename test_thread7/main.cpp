// #include "thread.hpp"
// #include <unistd.h>
// #include <vector>

// int tickets = 1000;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// void Getticket() {
//     char name[64];
//     pthread_getname_np(pthread_self(), name, sizeof(name));
//     while (true) {
//         pthread_mutex_lock(&mutex);
//         if (tickets > 0) {
//             usleep(1000);
//             printf("%s get ticket: %d\n", name, tickets);
//             --tickets;
//             pthread_mutex_unlock(&mutex);
//         }
//         else {
//             pthread_mutex_unlock(&mutex);
//             break;
//         }
//     }
// }


// int main() {
//     pthread_mutex_t mutex;
//     pthread_mutex_init(&mutex, nullptr);

//     my::Thread t1(Getticket);
//     my::Thread t2(Getticket);
//     my::Thread t3(Getticket);
//     my::Thread t4(Getticket);

//     t1.Start();
//     t2.Start();
//     t3.Start();
//     t4.Start();

//     t1.Join();
//     t2.Join();
//     t3.Join();
//     t4.Join();

//     pthread_mutex_destroy(&mutex);
//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>

class thread_data
{
public:
    thread_data(const std::string& name, pthread_mutex_t* pmutex):_name(name), _pmutex(pmutex)
    {}

public:
    std::string _name;
    pthread_mutex_t* _pmutex;
};

int ticket = 1000;

void *route(void *arg)
{
    thread_data *td = static_cast<thread_data*>(arg);
    while ( 1 ) {
        pthread_mutex_lock(td->_pmutex);
        if ( ticket > 0 ) {
            usleep(1000);
            printf("%s sells ticket:%d\n", td->_name.c_str(), ticket);
            ticket--;
            pthread_mutex_unlock(td->_pmutex);
        } else {
            pthread_mutex_unlock(td->_pmutex);
            break;
        }
    }
    return nullptr;
}

int main( void )
{
    pthread_t t1, t2, t3, t4;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    thread_data td1("thread_1", &mutex);
    thread_data td2("thread_2", &mutex);
    thread_data td3("thread_3", &mutex);
    thread_data td4("thread_4", &mutex);

    pthread_create(&t1, NULL, route, (void*)&td1);
    pthread_create(&t2, NULL, route, (void*)&td2);
    pthread_create(&t3, NULL, route, (void*)&td3);
    pthread_create(&t4, NULL, route, (void*)&td4);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_mutex_destroy(&mutex);
}