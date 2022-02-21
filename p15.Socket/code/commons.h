#ifndef _COMMONS_H
#define _COMMONS_H

#include <fcntl.h>

int set_non_blocking(int fd)
{

    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;

    return fcntl(fd, F_SETFL, flags);
}

#endif