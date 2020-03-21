#define _REENTRANT

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 5

void *thread_function(void *arg);

void read_from_stdin();
pthread_mutex_t mutex;
char message[BUF_SIZE + 1];

int main(int argc, char const *argv[])
{
    pthread_t th;
    void *retval;

    int res;
    if ((res = pthread_mutex_init(&mutex, NULL)) != 0)
    {
        perror("mutex init error!");
        exit(res);
    }

    if (pthread_create(&th, NULL, thread_function, NULL) != 0)
    {
        perror("thread create error!");
        exit(-1);
    }

    printf("main thread ID: %lx\n", pthread_self());
    while (1)
    {
        read_from_stdin();
    }

    return 0;
}

void read_from_stdin()
{
    pthread_mutex_lock(&mutex);

    if (fgets(message, BUF_SIZE, stdin) == NULL)
    {
        perror("stdin read error");
        exit(errno);
    }
    printf("thread ID[%lx] data: %s\n", pthread_self(), message);
    pthread_mutex_unlock(&mutex);
    sleep(1);
}

void *thread_function(void *arg)
{
    printf("sub thread ID: %lx\n", pthread_self());
    while (1)
    {
        read_from_stdin();
    }
    return 0;
}
