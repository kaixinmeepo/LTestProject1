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
#include <sys/socket.h>

namespace FILE_TEST {
    namespace TestEnv1 {
        void Test() {
            /*! Printf netting result in *.lni format */
            char* filename = "Info.lni";
            FILE* fd;
            char content[4096];
            int i;
            time_t timep;
            /*! check if exist */
            if (access(filename, F_OK) == 0) {
                remove(filename);
            }
            /*! creat and opne */
            fd = fopen(filename, "a+");
            if (fd == NULL) {
                printf("[ NET ]:open file error\n");
                return;
            }
            /*! table head */
            time(&timep);
            sprintf(content, "%s\n", asctime(localtime(&timep)));
            fprintf(fd, content);
            fseek(fd, 0, SEEK_END);
            sprintf(content, "nodeIdx      addr  isValid  isConnected  isRouted  routeNodeIdx  netLayer  netQuality  upSNR  downSNR  timecost\n");
            fseek(fd, 0, SEEK_END);
            fprintf(fd, content);
            int MAX_SLAVE_NODE = 8;
            for (i = 1; i < MAX_SLAVE_NODE + 1; i++) {
                sprintf(content, "%7d  %8d  %7d  %11d  %8d  %12d  %8d  %10d  %5d  %7d  %8d\n",
                    i,
                    i * 10000 + 1,
                    i * 10000 + 2,
                    i * 10000 + 3,
                    i * 10000 + 4,
                    i * 10000 + 5,
                    i * 10000 + 6,
                    i * 10000 + 7,
                    i * 10000 + 8,
                    i * 10000 + 9,
                    i * 10000 + 10
                );
                fseek(fd, 0, SEEK_END);
                fprintf(fd, content);
                fflush(fd);
            }
            fclose(fd);
            printf("[ NET ]:Info.lni creat ok\n");
        }
    }
    namespace TestEnv2 {
        void Test() {
            const char* filename = "test";
            int fd = open(filename, O_WRONLY | O_CREAT, 0664);
            if (fd == -1) return;
            auto p_file = fdopen(fd, "a");  // Make a FILE*.
            if (p_file == NULL) {  // Man, we're screwed!
                close(fd);
                unlink(filename);  // Erase the half-baked evidence: an unusable log file
                return;
            }
            std::string message = "test_message";
            errno = 0;
            fwrite(message.c_str(), 1, (int)message.size(), p_file);
            message = "test_message2";
            fwrite(message.c_str(), 1, (int)message.size(), p_file);
            if (errno == ENOSPC) {  // disk full, stop writing to disk
                fclose(p_file);
                return;
            }
            fclose(p_file);
        }
    }
    namespace TestEnv3 {
        void Test() {
            std::string path_file = "gs001";
            auto filename = path_file + "_start.log";
            FILE* p_file = fopen(filename.c_str(), "a+");
            if (!p_file) {
                printf("[GMatrix]RecordStart: open file error.\n");
                return;
            }
            std::stringstream ss;
            time_t timep = 0;
            time(&timep);
            pid_t pid = getpid();
            ss << pid << " " << timep << "\n";
            std::string content = ss.str();
            fwrite(content.c_str(), 1, (int)content.size(), p_file);
            if (errno == ENOSPC) {  // disk full, stop writing to disk
                fclose(p_file);
                printf("[GMatrix]RecordStart: disk full.\n");
                return;
            }
            fclose(p_file);
        }
    }
    namespace TestEnv4 {
        void Test() {
            //输出进程信息
            pid_t pid = getpid();
            char strProcessPath[1024] = { 0 };
            if (readlink("/proc/self/exe", strProcessPath, 1024) <= 0) {
                return;
            }

            char* strProcessName = strrchr(strProcessPath, '/');
            if (!strProcessName) {
                printf("当前进程ID：%d\n", pid);
                printf("当前进程名：\n");
                printf("当前进程路径：%s\n", strProcessPath);
            }
            else {
                printf("当前进程ID：%d\n", pid);
                printf("当前进程名：%s\n", ++strProcessName);
                printf("当前进程路径：%s\n", strProcessPath);
            }
        }
    }
    namespace TestEnv5 {
        void Test1() {
            //dup2 重定向
            struct tm tm_cur;
            time_t timep = time(nullptr);
            localtime_r(&timep, &tm_cur);
            char path_buf[50] = { 0 };
            int ret = snprintf(path_buf, 50, "logs/%04d-%02d-%02d/", tm_cur.tm_year + 1900, tm_cur.tm_mon + 1, tm_cur.tm_mday);
            std::string path_str(path_buf);
            if (access(path_buf, F_OK) != 0) {
                std::string command;
                command += "mkdir -p " + path_str;
                system(command.c_str());
            }
            snprintf(path_buf + ret, 50, "test_%02d_%02d_%02d.log", tm_cur.tm_hour, tm_cur.tm_min, tm_cur.tm_sec);
            int fd = open(path_buf, O_WRONLY | O_CREAT, 0644);
            char link_cmd_buf[70] = { 0 };
            snprintf(link_cmd_buf, 70, "ln -sf %s %s", path_buf, "gs.link");
            system(link_cmd_buf);

            int stdout_fd = dup(STDOUT_FILENO);
            int stderr_fd = dup(STDERR_FILENO);

            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            setvbuf(stdout, nullptr, _IOLBF, 0);
            printf("bianchengzhuji\n");
            printf("shouwangxiansheng\n");
            printf("bianchengzhuji\n");
            printf("shouwangxiansheng\n");

            sleep(60);
            char start_log[30] = { 0 };
            snprintf(start_log, 30, "%s start. s_pid:%d\n", "Test", getpid());
            std::string str = "write old fd.\n";
            write(stdout_fd, start_log, 30);
            write(stderr_fd, start_log, 30);

            int _old_stdout = dup(STDOUT_FILENO);
            int _old_stderr = dup(STDERR_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            printf("停止屏幕输出\n");

            dup2(_old_stdout, STDOUT_FILENO);
            dup2(_old_stderr, STDERR_FILENO);
            close(_old_stdout);
            close(_old_stderr);
            printf("开启屏幕输出\n");
        }
        void Test2() {
            int fd = open("test.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            int len = tee(STDIN_FILENO, STDOUT_FILENO, INT_MAX, SPLICE_F_NONBLOCK);
            printf("bianchengzhuji\n");
            printf("shouwangxiansheng\n");
            std::cerr << "test err." << std::endl;
        }
        void Test3() {
            /*splice()和tee()实现将文件"./1.txt"同时拷贝到文件"./2.txt"和"./3.txt"中*/
            int fd1 = open("./1.txt", O_RDONLY);
            int fd2 = open("./2.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
            int fd3 = open("./3.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

            int _old_stdout = dup(STDOUT_FILENO);
            dup2(fd1, STDOUT_FILENO);
            dup2(_old_stdout, STDOUT_FILENO);
            /*用于向"./2.txt"输入数据*/
            int pipefd2[2];
            /*用于向"./3.txt"输入数据*/
            int pipefd3[2];
            pipe(pipefd2);
            pipe(pipefd3);

            while (true) {
                /*将fd1文件的内容输入管道pipefd2中*/
                splice(fd1, NULL, pipefd2[1], NULL, 32768, SPLICE_F_MORE);
                /*将管道pipefd2的内容复制到管道pipefd3中，不消耗管道pipefd2上的数据，管道pipefd2上的数据可以用于后续操作*/
                tee(pipefd2[0], pipefd3[1], 10086, SPLICE_F_NONBLOCK);
                /*将管道pipefd2的内容写入fd2文件中*/
                splice(pipefd2[0], NULL, fd2, NULL, 32768, SPLICE_F_MORE);
                /*将管道pipefd3的内容写入fd3文件中*/
                splice(pipefd3[0], NULL, STDOUT_FILENO, NULL, 32768, SPLICE_F_MORE);
                sleep(1);
            }


            close(fd1);
            close(fd2);
            close(pipefd2[0]);
            close(pipefd2[1]);
        }
        void ClientTest() {
            for (int i = 0; i != 30; ++i) {
                printf("splice(fd1, NULL, pipefd2[1], NULL, 10086, SPLICE_F_MORE);splice(pipefd2[0], NULL, fd2, NULL, 10086, SPLICE_F_MORE);%d", i);
                sleep(1);
            }
        }
    }
    namespace TestEnv6 {
        void Test1() {
            //创建软连接
            int ret = symlink("x", "y");
            if (ret == 0)
                std::cout << "create ok" << std::endl;
            else
                std::cout << "create failed, " << strerror(errno) << std::endl;
        }
        void Test2() {
            //删除软连接
            int ret = unlink("y");
            if (ret == 0)
                std::cout << "unlink ok" << std::endl;
            else
                std::cout << "unlink failed, " << strerror(errno) << std::endl;
        }
        void Test3() {
            //读软连接
            enum { BUFFERSIZE = 1024 };
            char buf[BUFFERSIZE];
            ssize_t len = readlink("y", buf, sizeof(buf) - 1);
            if (len != -1)
            {
                buf[len] = '\0';
                std::string str = buf;
                std::cout << str << std::endl;
            }
            else {
                std::cout << "readlink failed, " << strerror(errno) << std::endl;
            }
        }
        void Test() {
            int op = 0;
            std::cin >> op;
            switch (op) {
            case 1: Test1(); break;
            case 2: Test2(); break;
            case 3: Test3(); break;
            default: break;
            }
        }
    }
    void Test() {
        //TestEnv1::Test();
        //TestEnv2::Test();
        //TestEnv3::Test();
        //TestEnv4::Test();
        //TestEnv5::Test1();
        //TestEnv6::Test();
    }
}
