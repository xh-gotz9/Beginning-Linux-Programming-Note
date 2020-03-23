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

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 3000;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    result = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        perror("bind error");
    }

    listen(server_sock, 128);

    int clientfd;
    struct sockaddr_in client_addr;
    int client_len;
    char buf[BUFSIZ];

    while (1)
    {
        printf("server waiting...\n");
        if ((clientfd = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        stpcpy(buf, "welcome to connect server!\n");
        write(clientfd, buf, strlen(buf));
        strcpy(buf, "bye-bye!\n");
        write(clientfd, buf, strlen(buf));

        printf("close socket!\n");
        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
    }

    return 0;
}
