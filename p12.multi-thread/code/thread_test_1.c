// 定义可重入宏
#define _REENTRANT

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

void *thread_function(void *arg);

char *message = "11CharsStr";

int main(int argc, char const *argv[])
{
    pthread_t th;
    void *retval;

    printf("message ptr: %p\n", message);

    if (pthread_create(&th, NULL, thread_function, message) != 0)
    {
        perror("create thread error!");
        exit(errno);
    }
    printf("createe thread success!\n");

    printf("waiting for thread...\n");
    if (pthread_join(th, &retval) != 0)
    {
        perror("join thread error!");
        exit(errno);
    }

    printf("thread joined.\narg ptr: %p\nreturned ptr: %p\nreturn value: %s\n", retval, retval, (char *)retval);

    return 0;
}

void *thread_function(void *arg)
{

    printf("thread started...\n");

    sleep(3);

    if (arg == NULL)
    {
        pthread_exit(NULL);
        // return NULL;
    }
    printf("message: %s\n", (char *)arg);

    return arg;
}
