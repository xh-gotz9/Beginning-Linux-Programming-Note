#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int server_sock;
    struct sockaddr_in addr;
    int result;

    // 服务器绑定地址
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 3000;

    // 创建 socket fd
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定到指定地址
    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    // 监听连接请求, 创建请求队列
    listen(server_sock, 128);

    int clientfd;
    struct sockaddr_in client_addr;
    int client_len;
    char buf[BUFSIZ];

    while (1)
    {
        printf("server waiting...\n");
        // 接受连接
        if ((clientfd = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        // 传输数据
        strcpy(buf, "welcome to connect server!\n");
        write(clientfd, buf, strlen(buf));
        strcpy(buf, "bye-bye!\n");
        write(clientfd, buf, strlen(buf));

        printf("close socket!\n");
#ifdef _SOCK_SHUTDOWN
        shutdown(clientfd, SHUT_RDWR);
#endif
        // close(clientfd);
    }

    return 0;
}
