#pragma once
#include <iostream>

struct CC;
struct CB;
int Testmain(int argc, char* argv[]);

struct CA {
    int a = 0;
    CA() {
        std::cout << "CA()" << this << " " << &a << std::endl;
    }
    virtual ~CA() {
    }
    virtual CB* GetBBase() { return nullptr; }
};
struct CB {
    int b = 0;
    CB() {
        std::cout << "CB()" << this << " " << &b << std::endl;
    }
    virtual ~CB() {

    }
    virtual void Test(CB** cc_ptr) {
        *cc_ptr = this;
    }
    virtual void VTest() {
        std::cout << "CB::VTest" << this << " " << &b << std::endl;
    }
};
struct Test {
    static CC* GetMng(CB* ca) { return (CC*)ca; }
};
struct CC : public CA, public CB {
    int c = 0;
    CC() {
        std::cout << "CC()" << this << " " << &c << std::endl;
    }
    void VTest() override {
        std::cout << "CC::VTest" << this << " " << &c << std::endl;
    }
    CB* GetBBase() override { return this; }
};
