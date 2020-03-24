#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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
    char buf[BUFSIZ];
    int nbytes;

#ifdef _OP_READ
#ifdef _SET_KEEPALIVE
    printf("set keepalive\n");
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1)
    {
        perror("set keepalive error");
        exit(EXIT_FAILURE);
    }
#endif
    while ((nbytes = read(sockfd, buf, BUFSIZ)) > 0)
#else
    while ((nbytes = recv(sockfd, buf, BUFSIZ, MSG_WAITALL)) > 0)
#endif
    {

#ifdef _OP_READ
        printf("read[len=%d]...\n", nbytes);
#else
        printf("recv[len=%d]...\n", nbytes);
#endif
        fflush(stdout);

        write(1, buf, nbytes);
        sleep(1);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
