#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../inc/bignum.h"

#define FIB_DEV "/dev/fibonacci"

int main(int argc, char **argv)
{
    int offset = 500; /* TODO: try test something bigger than the limit */
    char buf[300];


    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        *buf = *argv[1];
        lseek(fd, i, SEEK_SET);
        read(fd, buf, sizeof(char) * 300);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
    }
    for (int i = offset; i >= 0; i--) {
        *buf = *argv[1];
        lseek(fd, i, SEEK_SET);
        read(fd, buf, sizeof(char) * 300);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
    }
    close(fd);
    return 0;
}
