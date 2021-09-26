#include "file_test.h"
#include <iostream>
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

namespace STRING_PARSE_TEST {
    namespace TestEnv1 {
        static void Convert(const char* str, unsigned int& ui) {
            //        stringstream ss;
            //        ss << str;
            //        ss >> ui;
            int i = atoi(str);
            if (i < 0) {
                i = 0;
            }
            ui = (unsigned int)i;
        }
        static void Convert(const char* str, float& f) {
            f = atof(str);
        }
        static void Convert(const char* str, int& i) {
            i = atoi(str);
        }
        static void Convert(const char* str, std::string& i) {
            i = str;
        }
        template <class T>
        static void ParseArray(const std::string& str, std::vector<T>& arr) {
            size_t end = str.find_first_of(";");
            if (end == std::string::npos) {
                end = str.length();
            }
            size_t begin = 0;  // ignore特殊标记,例如,#,$表示是用float还是int
            while (begin < end) {
                size_t End = str.find_first_of(",", begin);  //以前Excel的
                T      i;
                if (End == std::string::npos) {
                    auto content = str.substr(begin, End - begin);
                    if (content.size()) {
                        Convert(content.c_str(), i);
                        arr.push_back(i);
                    }
                    break;
                }
                auto content = str.substr(begin, End - begin);
                if (content.size()) {
                    Convert(content.c_str(), i);
                    arr.push_back(i);
                }
                begin = End + 1;
            }
        }
        void Test() {
            std::string test = "1,2,3,4";
            std::vector<std::string> arr;
            ParseArray(test, arr);
        }
    }
    void Test() {
        //TestEnv1::Test();
    }
}
