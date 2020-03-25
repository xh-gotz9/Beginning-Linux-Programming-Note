#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in address;
    int result;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 3000;

    result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (result == -1)
    {
        perror("connect error");
        exit(EXIT_FAILURE);
    }
    char buf[BUFSIZ + 1] = {[BUFSIZ] = 0};
    int nbytes;

#ifdef _NON_BLOCK
    printf("set socket nonblock\n");
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK | flags);
#endif

#ifdef _SET_KEEPALIVE
    printf("set keepalive\n");
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1)
    {
        perror("set keepalive error");
        exit(EXIT_FAILURE);
    }
#endif

#ifdef _NOTIFY_SERVER
    printf("send notify server message\n");
    strcpy(buf, "hello, I'm coming!");
    write(sockfd, buf, strlen(buf));
#endif

read_socket:

#ifdef _OP_READ
    while ((nbytes = read(sockfd, buf, BUFSIZ)) > 0)
#else
    while ((nbytes = recv(sockfd, buf, BUFSIZ, MSG_WAITALL)) > 0)
#endif
    {

#ifdef _OP_READ
        printf("read[len=%d]:", nbytes);
#else
        printf("recv[len=%d]: ", nbytes);
#endif
        fflush(stdout);

        write(1, buf, nbytes);
        sleep(1);
    }

#ifdef _NON_BLOCK
    // 在使用 read 读取非阻塞模式 socket 会被阻塞时, 会发生错误并返回
    printf("check reason of break loop\n");
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
        printf("loop break because of EAGAIN or EWOULDBLOCK\n");
        sleep(1);
        goto read_socket;
    }
#endif

    close(sockfd);

    return EXIT_SUCCESS;
}
