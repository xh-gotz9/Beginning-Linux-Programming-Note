#include "commons.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int server_sock;
    struct sockaddr_in addr;
    int result;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(3000);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (set_non_blocking(server_sock) == -1)
    {
        perror("set fd non-blocking failed");
        exit(EXIT_FAILURE);
    }

    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 128);

    char buf[BUFSIZ + 1] = {[BUFSIZ] = 0};

    fd_set read_fds, write_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &read_fds);

    if (fork() == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid();

    fprintf(stdout, "select process pid: %d\n", pid);

    while (1)
    {
        int res;

        fprintf(stdout, "select waiting...\n");

        res = select(FD_SETSIZE, &read_fds, &write_fds, NULL, NULL);
        if (res == -1)
        {
            perror("select error");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "pid[%d] woken up\n", pid);

        if (res == 0)
        {
            fprintf(stdout, "empty loop\n");
            continue;
        }

        // 检查 fd
        for (int fd = 0; fd < FD_SETSIZE; fd++)
        {
            if (FD_ISSET(fd, &read_fds))
            {
                if (fd == server_sock)
                {
                    // 处理服务器 socket
                    int clientfd;
                    struct sockaddr_in client_addr;
                    int client_len;
                    // accept
                    if ((clientfd = accept(fd, (struct sockaddr *)&client_addr, &client_len)) == -1)
                    {
                        printf("error: %s\n", strerror(errno));

                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            fprintf(stdout, "empty accept\n");
                            continue;
                        }

                        perror("accept error");
                        exit(EXIT_FAILURE);
                    }

                    fprintf(stdout, "pid [%d] accepted connection\n", pid);

                    // close client
                    close(clientfd);
                }
            }
        }
    }

    return 0;
}
