/*************************************
* Lab 4
* Name:
* Student No:
* Lab Group:
*************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "shmheap.h"

shmheap_memory_handle shmheap_create(const char *name, size_t len) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, len);

    void *ptr = mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    shmheap_memory_handle *handle = malloc(sizeof(shmheap_memory_handle));
    handle->shmheap_id = shm_fd;
    handle->size = len;
    handle->ptr = ptr;
    handle->name = name;
    handle->offset = 0;

    return *handle;
}

shmheap_memory_handle shmheap_connect(const char *name) {
    int shm_fd;

    shm_fd = shm_open(name, O_RDWR, 0666);

    struct stat st;
    fstat(shm_fd, &st);

    ftruncate(shm_fd, st.st_size);

    void *ptr = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    shmheap_memory_handle *handle = malloc(sizeof(shmheap_memory_handle));
    handle->shmheap_id = shm_fd;
    handle->size = st.st_size;
    handle->ptr = ptr;
    hanle->name = name;
    handle->offset = 0;

    return *handle;
}

void shmheap_disconnect(shmheap_memory_handle mem) {
    munmap(mem.ptr, mem.size);
}

void shmheap_destroy(const char *name, shmheap_memory_handle mem) {
    munmap(mem.ptr, mem.size);
    shm_unlink(name);
}

void *shmheap_underlying(shmheap_memory_handle mem) {
    /* TODO */
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    return mem.ptr;
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    /* TODO */
}

shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    shmheap_object_handle *handle = malloc(sizeof(shmheap_object_handle));

    handle->ptr = ptr;
    handle->shmheap_id = mem.shmheap_id;

    return *handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    void *p = mem.ptr;
    return p;
}
