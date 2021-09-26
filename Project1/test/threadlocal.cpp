#include <thread>
#include <iostream>

//有且只有 thread_local 关键字修饰的变量具有线程（thread）周期，这些变量在线程开始的时候被生成，在线程结束的时候被销毁，并且每一个线程都拥有一个独立的变量实例。
//thread_local 一般用于需要保证线程安全的函数中。
//需要注意的一点是，如果类的成员函数内定义了 thread_local 变量，则对于同一个线程内的该类的多个对象都会共享一个变量实例，并且只会在第一次执行这个成员函数时初始化这个变量实例，这一点是跟类的静态成员变量类似的。

namespace THR_LOCAL {
    thread_local int g_n = 1;

    void f()
    {
        g_n++;
        printf("id=%d, n=%d\n", std::this_thread::get_id(), g_n);
    }

    void foo()
    {
        thread_local int i = 0;
        printf("id=%d, i=%d\n", std::this_thread::get_id(), i);
        i++;
    }

    void f2()
    {
        foo();
        foo();
    }

    int Test1()
    {
        printf("init id=%d, n=%d\n", std::this_thread::get_id(), g_n);
        g_n++;
        f();
        std::thread t1(f);
        std::thread t2(f);

        t1.join();
        t2.join();


        f2();
        std::thread t4(f2);
        std::thread t5(f2);

        t4.join();
        t5.join();
        return 0;
    }

    class A {
    public:
        A() {}
        ~A() {}

        void test(const std::string& name) {
            thread_local int count = 0;
            ++count;
            std::cout << name << ": " << count << std::endl;
        }
        void test2(const std::string& name) {
            static int count = 0;
            ++count;
            std::cout << name << ": " << count << std::endl;
        }
    };

    void func(const std::string& name) {
        A a1;
        a1.test(name);
        a1.test(name);
        A a2;
        a2.test(name);
        a2.test(name);
    }

    int Test2() {
        std::thread t1(func, "t1");
        t1.join();
        std::thread t2(func, "t2");
        t2.join();
        return 0;
    }

    void func2(const std::string& name) {
        A a1;
        a1.test2(name);
        a1.test2(name);
        A a2;
        a2.test2(name);
        a2.test2(name);
    }
    int Test3() {
        std::thread t1(func2, "t1");
        t1.join();
        std::thread t2(func2, "t2");
        t2.join();
        return 0;
    }
    int Test() {
        //Test1();
        //Test2();
        //Test3();
    }
}
