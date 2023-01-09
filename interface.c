#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void close_on_fail(ssize_t bytes, const char* msg, int fd)
{
    if (bytes < 0)
    {
        fprintf(stderr, "%s", msg);
        close(fd);
        exit(-1);
    }
}