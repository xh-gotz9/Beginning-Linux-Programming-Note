#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 20

int main(int argc, char const *argv[])
{
    FILE *rpipe;

    if (!(rpipe = popen("uname -a", "r")))
    {
        perror("open pipe error!");
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, rpipe))
    {
        printf("%s\n", buf);
    }

    return 0;
}
