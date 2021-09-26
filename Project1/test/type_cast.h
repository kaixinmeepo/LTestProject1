#pragma once
#include <iostream>

struct AClass {
    int a = 0;
    AClass() {
        std::cout << "AClass" << std::endl;
    }
    virtual ~AClass() {
        std::cout << "~AClass" << std::endl;
    }
};

struct BClass : public AClass {
    int b = 1;
    BClass() {
        std::cout << "BClass" << std::endl;
    }
    ~BClass() {
        std::cout << "~BClass" << std::endl;
    }
};

struct CClass : public AClass {
    int c = 2;
    BClass b;
    CClass() {
        std::cout << "CClass" << std::endl;
    }
    ~CClass() {
        std::cout << "~CClass" << std::endl;
    }
};

namespace TYPE_CAST {
    void Test();
}
