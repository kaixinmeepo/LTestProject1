#include "socket.h"
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
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>  
#include <sys/socket.h>  
#include <sys/un.h>  
#include <errno.h>  
#include <string.h>  
#include <unistd.h>  
#include <ctype.h>   

namespace SOCKET_TEST {
    // 共享内存
    namespace TestEnv1
    {
        const int N = 64;
        typedef struct {
            pid_t pid;
            char buf[N];
        } SHM;

        void handler(int signo) {
            printf("get signal\n");
            return;
        }

        void TestRead() {
            key_t key;
            int shmid;
            SHM* p;
            pid_t pid;

            if ((key = ftok(".", 'm')) < 0)
            {
                perror("fail to ftok");
                exit(-1);
            }

            signal(SIGUSR1, handler);//注册一个信号处理函数
            if ((shmid = shmget(key, sizeof(SHM), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
            {
                if (EEXIST == errno)//存在则直接打开
                {
                    shmid = shmget(key, sizeof(SHM), 0666);
                    p = (SHM*)shmat(shmid, NULL, 0);
                    pid = p->pid;
                    p->pid = getpid();//把自己的pid写到共享内存
                    kill(pid, SIGUSR1);
                }
                else//出错
                {
                    perror("fail to shmget");
                    exit(-1);
                }
            }
            else//成功
            {
                p = (SHM*)shmat(shmid, NULL, 0);
                p->pid = getpid();
                pause();
                pid = p->pid;//得到写端进程的pid
            }

            while (1)
            {
                pause();//阻塞，等待信号
                if (strcmp(p->buf, "quit\n") == 0) exit(0);//输入"quit结束"
                printf("read from shm : %s", p->buf);
                kill(pid, SIGUSR1);//向写进程发SIGUSR1信号
            }
        }
        void TestWrite() {
            key_t key;
            int shmid;
            SHM* p;
            pid_t pid;

            if ((key = ftok(".", 'm')) < 0)
            {
                perror("fail to ftok");
                exit(-1);
            }

            signal(SIGUSR1, handler);               //  注册一个信号处理函数
            if ((shmid = shmget(key, sizeof(SHM), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
            {
                if (EEXIST == errno)                //  存在则直接打开
                {
                    shmid = shmget(key, sizeof(SHM), 0666);

                    p = (SHM*)shmat(shmid, NULL, 0);

                    pid = p->pid;
                    p->pid = getpid();
                    kill(pid, SIGUSR1);
                }
                else//出错
                {
                    perror("fail to shmget");
                    exit(-1);
                }
            }
            else//成功
            {

                p = (SHM*)shmat(shmid, NULL, 0);
                p->pid = getpid();                  //  把自己的pid写到共享内存
                pause();
                pid = p->pid;                       //  得到读端进程的pid

            }

            while (1)
            {
                printf("write to shm : ");
                fgets(p->buf, N, stdin);            //  接收输入
                kill(pid, SIGUSR1);                 //  向读进程发SIGUSR1信号
                if (strcmp(p->buf, "quit\n") == 0) break;
                pause();                            //  阻塞，等待信号
            }
            shmdt(p);
            shmctl(shmid, IPC_RMID, NULL);          //  删除共享内存
        }
        void Test(int argn, char** argv) {
            if (argn < 2) {
                return;
            }
            int op = atoi(argv[1]);
            if (op == 1) {
                TestWrite();
            }
            if (op == 2) {
                TestRead();
            }
        }
    }
    //Unix domain socket 又叫 IPC(inter-process communication 进程间通信) socket，用于实现同一主机上的进程间通信。socket 原本是为网络通讯设计的，但后来在 socket 的框架上发展出一种 IPC 机制，就是 UNIX domain socket。虽然网络 socket 也可用于同一台主机的进程间通讯(通过 loopback 地址 127.0.0.1)，但是 UNIX domain socket 用于 IPC 更有效率：不需要经过网络协议栈，不需要打包拆包、计算校验和、维护序号和应答等，只是将应用层数据从一个进程拷贝到另一个进程。这是因为，IPC 机制本质上是可靠的通讯，而网络协议是为不可靠的通讯设计的。
    //UNIX domain socket 是全双工的，API 接口语义丰富，相比其它 IPC 机制有明显的优越性，目前已成为使用最广泛的 IPC 机制，比如 X Window 服务器和 GUI 程序之间就是通过 UNIX domain socket 通讯的。
    //Unix domain socket 是 POSIX 标准中的一个组件，所以不要被名字迷惑，linux 系统也是支持它的。
    namespace TestEnv2 {
        const int MAXLINE = 80;
        char* server_path = "server.socket";
        char* client_path = "client.socket";

        int TestServer1() {
            struct sockaddr_un serun, cliun;
            socklen_t cliun_len;
            int listenfd, connfd, size;
            char buf[MAXLINE];
            int i, n;

            if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
                perror("socket error");
                exit(1);
            }

            memset(&serun, 0, sizeof(serun));
            serun.sun_family = AF_UNIX;
            strcpy(serun.sun_path, server_path);
            size = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
            unlink(server_path);
            if (bind(listenfd, (struct sockaddr*)&serun, size) < 0) {
                perror("bind error");
                exit(1);
            }
            printf("UNIX domain socket bound\n");

            if (listen(listenfd, 20) < 0) {
                perror("listen error");
                exit(1);
            }
            printf("Accepting connections ...\n");

            while (1) {
                cliun_len = sizeof(cliun);
                if ((connfd = accept(listenfd, (struct sockaddr*) & cliun, &cliun_len)) < 0) {
                    perror("accept error");
                    continue;
                }

                while (1) {
                    n = read(connfd, buf, sizeof(buf));
                    if (n < 0) {
                        perror("read error");
                        break;
                    }
                    else if (n == 0) {
                        printf("EOF\n");
                        break;
                    }

                    printf("received: %s", buf);

                    for (i = 0; i < n; i++) {
                        buf[i] = toupper(buf[i]);
                    }
                    write(connfd, buf, n);
                }
                close(connfd);
            }
            close(listenfd);
            return 0;
        }

        int TestClient1() {
            struct  sockaddr_un cliun, serun;
            int len;
            char buf[100];
            int sockfd, n;

            if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
                perror("client socket error");
                exit(1);
            }

            // 一般显式调用bind函数，以便服务器区分不同客户端  
            memset(&cliun, 0, sizeof(cliun));
            cliun.sun_family = AF_UNIX;
            strcpy(cliun.sun_path, client_path);
            len = offsetof(struct sockaddr_un, sun_path) + strlen(cliun.sun_path);
            unlink(cliun.sun_path);
            if (bind(sockfd, (struct sockaddr*) & cliun, len) < 0) {
                perror("bind error");
                exit(1);
            }

            memset(&serun, 0, sizeof(serun));
            serun.sun_family = AF_UNIX;
            strcpy(serun.sun_path, server_path);
            len = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
            if (connect(sockfd, (struct sockaddr*)&serun, len) < 0) {
                perror("connect error");
                exit(1);
            }

            while (fgets(buf, MAXLINE, stdin) != NULL) {
                write(sockfd, buf, strlen(buf));
                n = read(sockfd, buf, MAXLINE);
                if (n < 0) {
                    printf("the other side has been closed.\n");
                }
                else {
                    write(STDOUT_FILENO, buf, n);
                }
            }
            close(sockfd);
            return 0;
        }
        void Test(int argn, char** argv) {
            if (argn < 2) {
                return;
            }
            int op = atoi(argv[1]);
            if (op == 1) {
                TestServer1();
            }
            if (op == 2) {
                TestClient1();
            }
        }
    }
    void Test(int argn, char** argv) {
        //TestEnv1::Test(argn, argv);
        //TestEnv2::Test(argn, argv);
    }
}
