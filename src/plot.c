#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../inc/bignum.h"

#define FIB_DEV "/dev/fibonacci"

int main(int argc, char *argv[])
{
    FILE *f;

    f = fopen(argv[1], "a+");
    char write_buf[] = "testing writing";
    int offset = 500; /* TODO: try test something bigger than the limit */
    uint64_t ret;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        uint64_t time;
        lseek(fd, i, SEEK_SET);
        char buf[300];
        *buf = *argv[1];
        if (*buf == '0' || *buf == '1' || *buf == '2') {
            ret = read(fd, buf, sizeof(char) * 300);
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%lu.\n",
                   i, ret);
        } else {
            read(fd, buf, sizeof(char) * 300);
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%s.\n",
                   i, buf);
        }

        time = (uint64_t) write(fd, write_buf, strlen(write_buf));
        printf("Writing to " FIB_DEV ", returned the sequence %lu\n", time);
        char text[40];
        snprintf(text, 8, "%lu", time);
        fprintf(f, "%s ", text);
    }
    fprintf(f, "\n");
    fclose(f);

    close(fd);
    return 0;
}
