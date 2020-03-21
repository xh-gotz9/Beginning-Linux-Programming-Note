#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 20

int main(int argc, char const *argv[])
{
    int data_processed;
    int file_pipes[2];

    const char some_data[] = "data";
    char buf[BUF_SIZE + 1];

    memset(buf, 0, sizeof(buf));

    if (pipe(file_pipes))
    {
        exit(EXIT_FAILURE);
    }

    data_processed = write(file_pipes[1], some_data, strlen(some_data));
    printf("wrote %d bytes\n", data_processed);
    data_processed = read(file_pipes[0], buf, BUF_SIZE);
    printf("read %d bytes: %s\n", data_processed, buf);

    exit(EXIT_SUCCESS);
}
