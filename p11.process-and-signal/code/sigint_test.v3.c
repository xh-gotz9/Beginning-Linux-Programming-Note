/**
 * register SIGINT handler by "signal"
 **/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int sig);

void signal_test();
void sigaction_test();
void process();

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        sigaction_test();
    }
    else
    {
        signal_test();
    }
    return 0;
}

struct sigaction sigint_oact, sighup_oact;

char *sigint_name = "SIGINT", *sighup_name = "SIGHUP";

void sig_handler(int sig)
{
    char *name;
    switch (sig)
    {
    case SIGINT:
        name = sigint_name;
        break;
    case SIGHUP:
        name = sighup_name;
        break;
    default:
        fprintf(stderr, "unknown sig");
        break;
    }

    fprintf(stderr, "handling %s...\n", name);
    // signal(SIGINT, SIG_DFL);
    process();
    fprintf(stderr, "handling %s finished...\n", name);
}

void signal_test()
{
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);

    process();
}

void sigaction_test()
{
    struct sigaction sigint_act, sighup_act;
    // sigint
    sigint_act.sa_handler = sig_handler;
    sigemptyset(&sigint_act.sa_mask);
    sigaddset(&sigint_act.sa_mask, SIGINT);
    sigint_act.sa_flags = 0;
    sigaction(SIGINT, &sigint_act, &sigint_oact);

    // sighup
    sighup_act.sa_handler = sig_handler;
    sigemptyset(&sighup_act.sa_mask);
    sigaddset(&sighup_act.sa_mask, SIGHUP);
    sighup_act.sa_flags = 0;
    sigaction(SIGHUP, &sighup_act, &sighup_oact);

    process();
}

void process()
{
    while (1)
    {
        fprintf(stdout, "processing...\n");
        sleep(1);
    }
}