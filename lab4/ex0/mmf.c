/*************************************
* Lab 4
* Name:
* Student No:
* Lab Group:
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include<sys/mman.h>
#include "mmf.h"

int fd;

void *mmf_create_or_open(const char *name, size_t sz) {
    if ((fd = open(name, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) < 0) {
        perror("Error opening file");
        exit(1);
    }

    ftruncate(fd, sz);

    char * addr = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    return addr;
}

void mmf_close(void *ptr, size_t sz) {
    munmap(ptr, sz);
    close(fd);
}
