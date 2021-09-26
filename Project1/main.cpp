#include <map>
#include <list>
#include <vector>
#include <memory>
#include <sstream>
#include <tuple>
#include <math.h>
#include <set>
#include <algorithm>
#include <time.h>
#include <float.h>
#include <string.h>
#include "main.h"
#include "test/test.h"
#include "test/rwlock.h"
#include "test/string_parse.h"
#include "test/thread.h"
#include "test/threadlocal.h"
#include "test/thread_pool.h"
#include "test/unique_lock.h"
#include "test/future_test.h"
#include "test/union_test.h"
#include "test/file_test.h"
#include "test/macro_test.h"
#include "test/type_cast.h"
#include "test/socket.h"


int main(int argn, char** argv) {
    RWLOCK::Test();
    THR_LOCAL::Test();
    THREAD_POOL::Test();
    UNIQUE_LOCK::Test();
    THREAD::Test();
    FUTURE_TEST::Test();
    UNION_TEST::Test();
    FILE_TEST::Test();
    MACRO_TEST::Test();
    STRING_PARSE_TEST::Test();
    TYPE_CAST::Test();
    SOCKET_TEST::Test(argn, argv);

    return 0;
}

 