#define _REENTRANT

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 20

void *thread_function(void *arg);

sem_t sem;
char message[BUF_SIZE + 1];

int main(int argc, char const *argv[])
{
    pthread_t th;
    void *retval;
    sem_init(&sem, 0, 0);

    if (pthread_create(&th, NULL, thread_function, NULL) != 0)
    {
        perror("thread create error!");
        exit(-1);
    }

    if (fgets(message, BUF_SIZE, stdin) != NULL)
    {
        sem_post(&sem);
    }

    pthread_join(th, &retval);

    return 0;
}

void *thread_function(void *arg)
{
    printf("thread running...\n");
    sem_wait(&sem);
    printf("data: %s\n", message);
    return 0;
}
