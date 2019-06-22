#include "buffer.h"
#include "co_routine.h"
#include <errno.h>

const char Buffer::kCRLF[] = "\r\n";

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    co_enable_hook_sys();
    char extrabuf[65536];

    const ssize_t n = read(fd, extrabuf, 65536);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else
    {
        append(extrabuf, n);
    }
    return n;
}
