#include <iostream>
#include <pthread.h>
#include <unistd.h>

namespace RWLOCK {
    using namespace std;
    pthread_rwlock_t rwlock;

    int shared_num = 100;

    void* thread_fun1(void* arg)
    {
        pthread_rwlock_wrlock(&rwlock);
        cout << "thread 1: " << shared_num << endl;
        sleep(2);
        pthread_rwlock_unlock(&rwlock);

        return (void*)1;
    }

    void* thread_fun2(void* arg)
    {
        pthread_rwlock_rdlock(&rwlock);
        cout << "thread 2: " << shared_num << endl;
        sleep(2);
        pthread_rwlock_unlock(&rwlock);
        return (void*)1;
    }

    int Test1()
    {
        pthread_t tid1, tid2;

        pthread_rwlock_init(&rwlock, NULL);

        pthread_create(&tid2, NULL, thread_fun2, NULL);
        pthread_create(&tid1, NULL, thread_fun1, NULL);


        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        pthread_rwlock_destroy(&rwlock);

        return 0;
    }
    int Test() {
        //Test1();
    }
}
