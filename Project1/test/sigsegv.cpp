#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <execinfo.h>

#define BACKTRACE_SIZE 16

void ShowStack(void)
{
    int i;
    void* buffer[BACKTRACE_SIZE];

    int n = backtrace(buffer, BACKTRACE_SIZE);
    printf("[%s]:[%d] n = %d\n", __func__, __LINE__, n);
    char** symbols = backtrace_symbols(buffer, n);
    if (NULL == symbols) {
        perror("backtrace symbols");
        exit(EXIT_FAILURE);
    }
    printf("[%s]:[%d]\n", __func__, __LINE__);
    for (i = 0; i < n; i++) {
        printf("%d: %s\n", i, symbols[i]);
    }

    free(symbols);
}

void sigsegv_handler(int signo)
{
    if (signo == SIGSEGV) {
        printf("Receive SIGSEGV signal\n");
        printf("-----call stack-----\n");
        ShowStack();
        exit(-1);
    }
    else {
        printf("this is sig %d", signo);
    }
}

int Testmain(int argc, char* argv[])
{
    printf("The code compile date:[%s]:[%s] !!!\n", __DATE__, __TIME__);
    signal(SIGSEGV, sigsegv_handler);
    int i = 0;
    char* p = NULL;
    while (1)
    {
        printf("%s:[%s]:[%d]\n", __DATE__, __func__, __LINE__);
        sleep(1);
        i++;
        if (3 == i) {
            *p = 0x55;
        }
    }
    return -1;
}