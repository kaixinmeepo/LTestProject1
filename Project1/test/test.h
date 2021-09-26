#pragma once
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <memory>

uint64_t GetCPUTickCount();
struct CalcTime {
    uint64_t begin[2] = {0};
    struct timeval tv_beg[2];
    void Begin() {
        gettimeofday(&tv_beg[0], nullptr);
        begin[0] = GetCPUTickCount();
    }
    void End() {
        begin[1] = GetCPUTickCount();
        gettimeofday(&tv_beg[1], nullptr);
    }
    void Dump() {
        std::cout << begin[1] - begin[0] << " " << tv_beg[1].tv_usec - tv_beg[0].tv_usec << std::endl;
    }
};



class CClass;
class RefTest {
public:
    RefTest();
    ~RefTest();
    CClass& GetSPeople() {
        return *_speople_manager;
    }
    void Init();
private:
    std::unique_ptr<CClass> _speople_manager;
};

