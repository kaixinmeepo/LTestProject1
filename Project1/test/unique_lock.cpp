#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

namespace UNIQUE_LOCK {
    // reference: http://www.cplusplus.com/reference/mutex/unique_lock/unique_lock/
    namespace TestEnv1  {
        std::mutex foo, bar;

        void task_a()
        {
            std::lock(foo, bar);         // simultaneous lock (prevents deadlock)
            std::unique_lock<std::mutex> lck1(foo, std::adopt_lock);
            std::unique_lock<std::mutex> lck2(bar, std::adopt_lock);
            std::cout << "task a\n";
            // (unlocked automatically on destruction of lck1 and lck2)
        }

        void task_b()
        {
            // unique_lock::unique_lock: Constructs a unique_lock
            // foo.lock(); bar.lock(); // replaced by:
            std::unique_lock<std::mutex> lck1, lck2;
            lck1 = std::unique_lock<std::mutex>(bar, std::defer_lock);
            lck2 = std::unique_lock<std::mutex>(foo, std::defer_lock);
            std::lock(lck1, lck2);       // simultaneous lock (prevents deadlock)
            std::cout << "task b\n";
            // (unlocked automatically on destruction of lck1 and lck2)
        }
        int Test()
        {
            std::thread th1(task_a);
            std::thread th2(task_b);

            th1.join();
            th2.join();

            return 0;
        }
    }
    namespace TestEnv2 {
        std::mutex mtx;           // mutex for critical section

        void print_thread_id(int id) {
            std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
            // critical section (exclusive access to std::cout signaled by locking lck):
            lck.lock();
            std::cout << "thread #" << id << '\n';
            lck.unlock();
        }

        int Test()
        {
            std::thread threads[10];
            // spawn 10 threads:
            for (int i = 0; i < 10; ++i)
                threads[i] = std::thread(print_thread_id, i + 1);

            for (auto& th : threads) th.join();

            return 0;
        }
    }

    namespace  TestEnv3 {
        struct Box {
            explicit Box(int num) : num_things{ num } {}

            int num_things;
            std::mutex m;
        };

        void transfer(Box& from, Box& to, int num)
        {
            // don't actually take the locks yet
            std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
            std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);

            // lock both unique_locks without deadlock
            std::lock(lock1, lock2);

            from.num_things -= num;
            to.num_things += num;

            std::cout << " thr_id 3 " << std::this_thread::get_id() << std::endl;
            // 'from.m' and 'to.m' mutexes unlocked in 'unique_lock' dtors
        }

        int Test()
        {
            Box acc1(100);
            Box acc2(50);

            std::thread t1(transfer, std::ref(acc1), std::ref(acc2), 10);
            std::thread t2(transfer, std::ref(acc2), std::ref(acc1), 5);

            t1.join();
            t2.join();
        }
    }

    void Test() {
        //TestEnv1::Test();
        //TestEnv2::Test(); 
        //TestEnv3::Test();
    }
} // namespace UNIQUE_LOCK
