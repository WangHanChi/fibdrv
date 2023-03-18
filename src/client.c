#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../inc/bignum.h"

#define FIB_DEV "/dev/fibonacci"

int main()
{
    // FILE *f;
    // f = fopen("data.txt", "a+");
    char buf[300];
    // char write_buf[] = "testing writing";
    int offset = 500; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        // uint64_t time;
        lseek(fd, i, SEEK_SET);
        read(fd, buf, sizeof(buf));
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
        // time = (uint64_t) write(fd, write_buf, strlen(write_buf));
        // //printf("Writing to " FIB_DEV ", returned the sequence %lu\n",
        // time); char text[40]; snprintf(text, 8, "%lu", time); fprintf(f, "%s
        // ", text);
    }
    for (int i = offset; i >= 0; i--) {
        // uint64_t time;
        lseek(fd, i, SEEK_SET);
        read(fd, buf, sizeof(buf));
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
        // time = (uint64_t) write(fd, write_buf, strlen(write_buf));
        // //printf("Writing to " FIB_DEV ", returned the sequence %lu\n",
        // time); char text[40]; snprintf(text, 8, "%lu", time); fprintf(f, "%s
        // ", text);
    }
    // fprintf(f, "\n");
    // fclose(f);
    close(fd);
    return 0;
}
