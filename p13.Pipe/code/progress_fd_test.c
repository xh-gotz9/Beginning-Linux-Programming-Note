#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>

int main(int argc, char const *argv[])
{
    int pipe_fds[2];

    if (pipe(pipe_fds))
    {
        perror("open pipe error");
        exit(EXIT_FAILURE);
    }
    printf("pipe fd[0]: %d, fd[1]: %d\n", pipe_fds[0], pipe_fds[1]);

    pid_t pid = fork();

    switch (pid)
    {
    case -1:
        perror("fork error");
        exit(EXIT_FAILURE);
        break;
    case 0:
        printf("child: exec ping...\n");
        // 启动 ping 程序, 程序启动后通过查看 /proc/%pid%/fd 目录检查 fd
        if (execlp("ping", "ping", "192.168.0.1", NULL))
        {
            perror("exec call error!");
            exit(EXIT_FAILURE);
        }
        break;
    default:
        printf("child pid: %d\n", pid);
        sleep(30);
        printf("raise SIGINT for PID[%d]\n", pid);

        // 结束子进程运行的 ping 程序
        if (kill(pid, SIGINT))
        {
            fprintf(stderr, "kill SIGINT to PID[%d] failed\n", pid);
            exit(EXIT_FAILURE);
        };
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
