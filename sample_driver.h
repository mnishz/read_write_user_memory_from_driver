#ifndef SAMPLE_DRIVER_H
#define SAMPLE_DRIVER_H

#include <linux/ioctl.h>

#define SAMPLE_DRIVER_IOC_TYPE 's'
#define USER_PROC_A _IOW(SAMPLE_DRIVER_IOC_TYPE, 1, uintptr_t)
#define USER_PROC_B _IO(SAMPLE_DRIVER_IOC_TYPE, 2)

#endif
