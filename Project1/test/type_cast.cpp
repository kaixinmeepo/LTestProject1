#include "type_cast.h"

struct A {
    int a = 1;
    virtual ~A() {

    }
};
struct AA : public A {
    int aa = 11;
};
struct AB : public A {
    int ab = 12;
};
struct B {
    int b = 2;
    virtual ~B() {

    }
};
struct A_B : public A, public B {
    int a_b = 102;
};
namespace TYPE_CAST {
    namespace TestEnv1 {
        void Test() {
            AClass* p_a = new CClass();

            BClass* p_b = static_cast<BClass*>(p_a);
            if (p_b) {
                std::cout << "p_b:" << p_b->b << std::endl;
            }
            std::cout << "p_a:" << p_a->a << std::endl;

            A* test1 = new AB();
            auto test11 = (AA*)(test1);
            auto test12 = dynamic_cast<AA*>(test1);

            A_B* test2 = new A_B();
            auto test21 = (A*)(test2);
            auto test22 = (B*)(test2);
        }
    }
    namespace TestEnv2 {
        template<typename T>
        void Func1(T& value) {
            int i_value = *(int*)(&value);
            std::cout << value << std::endl;
            std::cout << i_value << std::endl;
        }
        void Test() {
            float test = 123.456;
            short test1 = 123;
            Func1(test);
            Func1(test1);
        }
    }
    void Test() {
        //TestEnv2::Test();
    }
}
