#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int sig)
{
    printf("handling signal...\n");
}

int main(int argc, char const *argv[])
{
    // 设置信号处理函数
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    // 设置屏蔽信号 SIGINT
    sigset_t local_mask;
    sigemptyset(&local_mask);
    sigaddset(&local_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &local_mask, NULL);

    // 发送SIGINT信号
    for (size_t i = 1; i < 11; i++)
    {
        printf("processing %ld... please raise SIGINT...\n", i);
        sleep(1);
    }

    sigset_t set;
    sigemptyset(&set);

    // 使用 sigsuspend 暂时解除屏蔽
    sigsuspend(&set);

    // 等待
    for (size_t i = 1; i < 11; i++)
    {
        printf("waiting %ld...\n", i);
        sleep(2);
    }

    return 0;
}
