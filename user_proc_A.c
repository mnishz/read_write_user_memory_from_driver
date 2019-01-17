#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <stdint.h>

#include "sample_driver.h"

uintptr_t GetPhysicalAddress(uintptr_t virtualAddress);

int main(void)
{
    int returnValue = 0;

    int* const flagPtr = (int* const)malloc(sizeof(*flagPtr));
    if (flagPtr == NULL) {
        perror("malloc: ");
        returnValue = 1;
        goto malloc_error_handler;
    }

    int const mlockResult = mlock(flagPtr, sizeof(*flagPtr));
    if (mlockResult == -1) {
        perror("mlock: ");
        returnValue = 1;
        goto mlock_error_handler;
    }

    *flagPtr = 0;
    uintptr_t const physAddr = GetPhysicalAddress((uintptr_t)flagPtr);
    if (physAddr == 0) {
        returnValue = 1;
        goto phys_addr_error_handler;
    }

    int const fd = open("/dev/sample_driver0", O_RDWR);
    if (fd == -1) {
        perror("open: ");
        returnValue = 1;
        goto open_error_handler;
    }

    if (ioctl(fd, USER_PROC_A, &physAddr) == -1) {
        perror("ioctl: ");
        returnValue = 1;
        goto ioctl_error_handler;
    }

    while (*flagPtr == 0) {
        printf("*flagPtr: %d\n", *flagPtr);
        sleep(1);
    }

    printf("*flagPtr: %d\n", *flagPtr);

ioctl_error_handler:
    if (fd != -1) close(fd);
open_error_handler:
phys_addr_error_handler:
    if (mlockResult != -1) munlock(flagPtr, sizeof(*flagPtr));
mlock_error_handler:
    if (flagPtr != NULL) free(flagPtr);
malloc_error_handler:

    return returnValue;
}

uintptr_t GetPhysicalAddress(uintptr_t const virtualAddress)
{
    uintptr_t physAddr = 0;
    FILE* pagemap;

    if ((pagemap = fopen("/proc/self/pagemap", "r"))) {
        uint64_t virtPageEntry;
        off_t offset = (virtualAddress / sysconf(_SC_PAGESIZE)) * sizeof(virtPageEntry);
        if (lseek(fileno(pagemap), offset, SEEK_SET) == offset) {
            if (fread(&virtPageEntry, sizeof(virtPageEntry), 1, pagemap)) {
                if (virtPageEntry & (1ull << 63)) {
                    physAddr = virtPageEntry & ((1ull << 54) - 1);
                    if (physAddr == 0) {
                        printf("CAP_SYS_ADMIN is required. This proc must be executed by root.\n");
                    } else {
                        physAddr = physAddr * sysconf(_SC_PAGESIZE);
                        physAddr = physAddr | (virtualAddress & (sysconf(_SC_PAGESIZE) - 1));
                    }
                }
            }
        }
        fclose(pagemap);
    }

    return physAddr;
}
