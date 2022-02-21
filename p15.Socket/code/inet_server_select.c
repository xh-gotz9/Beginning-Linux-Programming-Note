#include "commons.h"

#include <stdlib.h>
#include <unistd.h>
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

    if (set_non_blocking(server_sock) == -1)
    {
        perror("set fd non-blocking");
        exit(EXIT_FAILURE);
    }

    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 128);

    char buf[BUFSIZ + 1] = {[BUFSIZ] = 0}; // 最后一位初始化为0

    fd_set read_fds, write_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &read_fds);

    while (1)
    {
        int res;
        res = select(FD_SETSIZE, &read_fds, &write_fds, NULL, NULL);
        if (res == -1)
        {
            perror("select error");
            close(server_sock);
            for (int fd = 0; fd < FD_SETSIZE; fd++)
            {
                if (FD_ISSET(fd, &read_fds))
                {
                    close(fd);
                    continue;
                }
                if (FD_ISSET(fd, &write_fds))
                {
                    close(fd);
                }
            }
            exit(EXIT_FAILURE);
        }

        if (res == 0)
            continue;

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
                        perror("accept error");
                        exit(EXIT_FAILURE);
                    }

                    // 添加到监听fdset中
                    FD_SET(clientfd, &read_fds);
                    FD_SET(clientfd, &write_fds);
                }
                else
                {
                    // 处理普通 client socket
                    int nbytes = read(fd, buf, BUFSIZ);
                    if (nbytes == -1)
                    {
                        perror("read failed");
                        FD_CLR(fd, &read_fds);
                        FD_CLR(fd, &write_fds);
                        close(fd);
                        fprintf(stdout, "remove client fd[%d]\n", fd);
                    }
                    else if (nbytes == 0)
                    {
                        // 读取到 0 数据. 说明出现错误, 关闭 socket
                        fprintf(stdout, "remote peer closed\n");
                        FD_CLR(fd, &read_fds);
                        FD_CLR(fd, &write_fds);
                        close(fd);
                        fprintf(stdout, "remove client fd[%d]\n", fd);
                    }
                    else
                    {
                        fprintf(stdout, "fd[%d] data recv:", fd);
                        fwrite(buf, 1, nbytes, stdout);
                        fprintf(stdout, "\n");
                        fflush(stdout);
                    }
                }
            }

            if (FD_ISSET(fd, &write_fds))
            {
                write(fd, "hello!\n", sizeof("hello!\n") - 1);
                close(fd);
                FD_CLR(fd, &read_fds);
                FD_CLR(fd, &write_fds);
            }
        }
    }

    return 0;
}
