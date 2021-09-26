#include "test.h"
#include "type_cast.h"

uint64_t GetCPUTickCount() {
    unsigned long high32 = 0, low32 = 0;
#ifdef WIN32 // WIN32
    _asm
    {
        RDTSC;
        mov high32, ebx;
        mov low32, eax;
    }
#else
    __asm__("RDTSC" : "=a"(low32), "=d"(high32));
#endif
    uint64_t counter = high32;
    counter = (counter << 32) + low32;
    return counter;
}

RefTest::RefTest() : _speople_manager(new CClass()) {
}
RefTest::~RefTest() {

}
void RefTest::Init() {
}