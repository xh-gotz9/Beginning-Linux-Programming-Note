#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

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

    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 128);

    char buf[BUFSIZ + 1] = {[BUFSIZ] = 0}; // 最后一位初始化为0

#define _DFL_FDSIZE (32)
    int fdsize = _DFL_FDSIZE;
    struct pollfd *fds = calloc(fdsize + 1, sizeof(struct pollfd));
    memset(fds, 0, sizeof(struct pollfd) * (fdsize + 1));

    // 监听 server fd
    fds[0].fd = server_sock;
    fds[0].events = POLLIN | POLLOUT;

    while (1)
    {
        printf("start poll...\n");
        int res = poll(fds, fdsize, 5000);
        if (res == -1)
        {
            perror("poll error");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        if (res == 0)
        {
            continue;
        }

        // handle fd events
        printf("handle fd events:\n");
        for (size_t i = 0; i < fdsize; i++)
        {

            int fd = fds[i].fd;
            short revents = fds[i].revents;
            if (fd == 0 || revents == 0)
            {
                // 空 pollfd 或无事件
                continue;
            }

            printf("fd[%d] - event[%d]\n", fd, revents);

            if (fd == server_sock && revents & POLLIN)
            {
                // accept socket
                int clientfd;
                printf("server socket readable.\n");
                clientfd = accept(server_sock, NULL, NULL);
                if (clientfd < -1)
                {
                    perror("accept error");
                    close(server_sock);
                    exit(EXIT_FAILURE);
                }

                struct pollfd *ptr = fds;
                while (ptr->fd != 0)
                {
                    ptr++;
                }

                int index = ptr - fds;

                // 扩容
                if (index == fdsize)
                {
                    fdsize <<= 2;
                    fds = reallocarray(fds, fdsize + 1, sizeof(struct pollfd));
                    memset((void *)&fds[fdsize >> 2], 0, sizeof(struct pollfd) * ((fdsize >> 2) + 1));
                }

                fds[index].fd = clientfd;
                fds[index].events = POLLIN | POLLOUT;
            }
            else
            {
                // 普通 socket 事件
                if (revents & POLLIN)
                {
                    int res = read(fd, buf, BUFSIZ);
                    if (res <= 0)
                    {
                        perror("read error");
                        goto close_and_clear_fd;
                    }
                    else
                    {
                        fprintf(stdout, "fd[%d] recv:", fd);
                        fwrite(buf, sizeof(char), res, stdout);
                        fwrite("\n", 1, 1, stdout);
                    }
                }

                if (revents & POLLOUT)
                {
                    char *s = "welcome and good-bye~";
                    write(fd, s, strlen(s));
                    goto close_and_clear_fd;
                }
            }

            if (revents & POLLERR || revents & POLLHUP || revents & POLLNVAL)
            {
                // report error
                (revents & POLLERR) ? printf("fd[%d] - POLLERR\n", fd) : 0;
                (revents & POLLHUP) ? printf("fd[%d] - POLLHUP\n", fd) : 0;
                (revents & POLLNVAL) ? printf("fd[%d] - POLLNVAL\n", fd) : 0;
                goto close_and_clear_fd;
            }

            continue;

        close_and_clear_fd:
            close(fd);
            memset(&fds[i], 0, sizeof(struct pollfd));
        }
    }

    return 0;
}
