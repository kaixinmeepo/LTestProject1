#include "macro_test.h"
#include <fcntl.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <limits.h>
#include <vector>
#include <thread>

namespace MACRO_TEST {
    namespace TestEnv1 {
#define _MACREO_SPLICE(x,y) x##y
#define MACREO_SPLICE(x,y) _MACREO_SPLICE(x,y)
#define VAR_LINE(x) MACREO_SPLICE(x, __LINE__)
//#define GS_LOG(t, x) int VAR_LINE(__lvl)=LOG_##t; if (!IsLogInited()) { VAR_LINE(__lvl)=LOG_STDOUT; } /*printf("GS_LOG:%d\n", VAR_LINE(__lvl));*/ LogMessage(VAR_LINE(__lvl), x)
#define GS_LOG(t, x) LogMessage(LOG_##t, x)
        void Test() {
            GS_LOG(DEBUG, "asdf.").P("123", 123);
        }
    }
    void Test() {
        //TestEnv1::Test();
    }
}

