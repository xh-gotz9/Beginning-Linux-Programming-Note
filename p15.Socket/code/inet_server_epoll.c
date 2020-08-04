#define _GNU_SOURCE
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
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int set_non_block(int fd)
{
    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1)
    {
        perror("fcntl");
        return -1;
    }
}

int main(int argc, char const *argv[])
{
    // 创建监听 fd
    int server_sock;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // 创建 epoll 对象
    int epfd = epoll_create1(0);
    struct epoll_event event;
    int event_size = 256;
    struct epoll_event events[256];

    event.data.fd = server_sock;
    event.events = EPOLLIN | EPOLLET;

    // server_sock 调整为非阻塞模式
    set_non_block(server_sock);

    // 绑定地址
    struct sockaddr_in addr;
    int result;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 3000;
    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    // 监听
    listen(server_sock, 128);

    // 添加 server fd 到 epoll 对象
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);

    struct sockaddr_in clientaddr;
    socklen_t client_addr_len;

    // I/O buffer
    char buf[BUFSIZ + 1] = {[BUFSIZ] = 0}; // 最后一位初始化为0

    while (1)
    {
        printf("start epoll...\n");
        int res = epoll_wait(epfd, events, event_size, 5000);

        // 错误处理
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
        printf("handle %d fd events:\n", res);
        for (size_t i = 0; i < res; i++)
        {

            int fd = events[i].data.fd;

            if (fd == server_sock) // 只注册了 POLLIN
            {
                int accept_fd;
                while (1)
                {
                    printf("accept socket\n");
                    accept_fd = accept(server_sock, (struct sockaddr *)&clientaddr, &client_addr_len);
                    // 错误处理
                    if (accept_fd < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            printf("accept end\n");
                            break;
                        }
                        else
                        {
                            printf("accept error\n");
                            break;
                        }
                    }

                    // 设置非阻塞模式
                    set_non_block(accept_fd);

                    // 添加 fd 到 epoll
                    event.data.fd = accept_fd;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;

                    printf("epoll - add new fd %d\n", accept_fd);
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, accept_fd, &event) == -1)
                    {
                        printf("epoll_ctl - add - error\n");
                    }
                }
            }
            else
            {
                if (events[i].events & EPOLLERR ||
                    events[i].events & EPOLLHUP)
                {
                    printf("error\n");
                    close(fd);
                    continue;
                }

                printf("original socket\n");
                // 普通 socket 事件
                if (events[i].events & EPOLLIN)
                {
                    while (1)
                    {
                        int nbytes = read(fd, buf, BUFSIZ);
                        if (nbytes <= 0)
                        {
                            if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                            {
                                break;
                            }
                            else
                            {
                                perror("read");
                                goto close_and_clear_fd;
                            }
                        }
                        else
                        {
                            // 在控制台输出数据
                            fprintf(stdout, "fd[%d] recv: ", fd);
                            fwrite(buf, sizeof(char), nbytes, stdout);
                            fwrite("\n", 1, 1, stdout);
                        }
                    }
                }

                if (events[i].events & EPOLLOUT)
                {
                    char *s = "welcome and good-bye~";
                    write(fd, s, strlen(s));
                    goto close_and_clear_fd;
                }
                continue;

            close_and_clear_fd:
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        }
    }
    return 0;
}
