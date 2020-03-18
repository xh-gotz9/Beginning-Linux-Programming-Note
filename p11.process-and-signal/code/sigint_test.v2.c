/**
 * register SIGINT handler by "sigaction"
 **/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct sigaction sigint_oact;

void sigint_handler(int sig)
{
    fprintf(stderr, "handling sigint...\n");
    // sigaction(SIGINT, &sigint_oact, 0);
    sleep(2);
    fprintf(stderr, "handling sigint finished...\n");
}

int main(int argc, char const *argv[])
{
    struct sigaction sigint_act;
    sigint_act.sa_handler = sigint_handler;
    sigemptyset(&sigint_act.sa_mask);
    sigaddset(&sigint_act.sa_mask, SIGINT);
    sigint_act.sa_flags = 0;
    sigaction(SIGINT, &sigint_act, &sigint_oact);

    while (1)
    {
        fprintf(stdout, "processing...\n");
        sleep(1);
    }

    return 0;
}
