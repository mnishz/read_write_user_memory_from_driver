#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "sample_driver.h"

int main(void)
{
    int returnValue = 0;

    int const fd = open("/dev/sample_driver0", O_RDWR);
    if (fd == -1) {
        perror("open: ");
        returnValue = 1;
        goto open_error_handler;
    }

    if (ioctl(fd, USER_PROC_B) == -1) {
        perror("ioctl: ");
        returnValue = 1;
        goto ioctl_error_handler;
    }

ioctl_error_handler:
    if (fd != -1) close(fd);
open_error_handler:

    return returnValue;
}
