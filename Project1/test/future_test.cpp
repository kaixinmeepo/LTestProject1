#include <iostream> 
#include <random> 
#include <atomic>
#include "thread.h"

namespace FUTURE_TEST {
    namespace TestEnv1 {
        bool is_prime(int x) {
            for (int i = 2; i < x; ++i) if (x % i == 0) return false;
            return true;
        }

        int Test1()
        {
            // call function asynchronously:
            std::future<bool> fut = std::async(is_prime, 444444443);

            // do something while waiting for function to set future:
            std::cout << "checking, please wait";
            //std::chrono::milliseconds span(100);
            //while (fut.wait_for(span) == std::future_status::timeout)
            //    std::cout << '.' << std::flush;

            bool x = fut.get();     // retrieve return value

            std::cout << "\n444444443 " << (x ? "is" : "is not") << " prime.\n";

            return 0;
        }
    }
    void Test() {
        //TestEnv1::Test1();
    }
}
