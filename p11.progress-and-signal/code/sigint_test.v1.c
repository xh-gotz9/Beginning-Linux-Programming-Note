/**
 * register SIGINT handler by "signal"
 **/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigint_handler(int sig)
{
    fprintf(stderr, "handling sigint...\n");
    // signal(SIGINT, SIG_DFL);
    sleep(2);
    fprintf(stderr, "handling finished...\n");
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, sigint_handler);
    while (1)
    {
        printf("processing...\n");
        sleep(1);
    }

    return 0;
}
