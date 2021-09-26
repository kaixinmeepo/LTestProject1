#pragma once
#include <iostream>
#include <sstream>

#define LOG_STDOUT 4
#define LOG_DEBUG 7

bool IsLogInited();
inline void GetCurTimeStr(char* szStr, int iLen) {
    time_t now = time(NULL);
    struct tm now_time;
    localtime_r(&now, &now_time);
    snprintf(szStr, iLen, "%02d:%02d:%02d", now_time.tm_hour, now_time.tm_min, now_time.tm_sec);
}

template <typename T>
inline void AppendLogValue(std::ostream& os, const T& v) {
    os << v;
}
class LogMessage : public std::stringstream {
    int  _type = 0;
    char _eva = '=';
public:
    LogMessage(int log_type, const char* title)
        : _type(log_type) {
        if (!IsLogInited()) {
            printf("IsLogInited false.");
        }
        *this << title;
    }
    ~LogMessage() {
        auto content = str();
        char szTime[64];
        GetCurTimeStr(szTime, sizeof(szTime));
        fprintf(stdout, "%s,%.*s\n", szTime, (int)content.size(), content.c_str());
    }
    template <class T>
    LogMessage& P(const char* attr, T v) {
        *this << attr << _eva;
        AppendLogValue(*this, v);
        return *this;
    }
};

class LogManager {
public:
    bool is_init;
    static LogManager& Instance() { static LogManager sInstance; return sInstance; }
};

inline bool IsLogInited() {
    return LogManager::Instance().is_init;
}

#define LOG_MESSAGE_DEBUG(x) LogMessage(LOG_DEBUG, (x))
#define LOG_MESSAGE_STD(x) LogMessage(LOG_STDOUT, (x))
#define LOG(t, x) LOG_MESSAGE_##t(x)

namespace MACRO_TEST {
    void Test();
}
