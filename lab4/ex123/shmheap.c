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
#include <stdio.h>
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
    /*
    bookkeep *first = malloc(sizeof(bookkeep));
    first->start = sizeof(bookkeep);
    first->end = len;
    first->free = 1;
    handle->bookkeep = first;
     */
    bookkeep *bookkeep_ptr = (bookkeep *) ptr;
    bookkeep_ptr->start = sizeof(bookkeep);
    bookkeep_ptr->end = len;
    bookkeep_ptr->free = 1;

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
    handle->name = name;

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
    return mem.ptr;
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    bookkeep *bookkeep_ptr = (bookkeep *) mem->ptr;
    if (bookkeep_ptr->end == mem.size - 1) {
        if (bookkeep_ptr->end - bookkeep_ptr->start >= sz && bookkeep_ptr->free) {
            int next = bookkeep_ptr->start;
            (bookkeep_ptr + next)->start = bookkeep_ptr->end + 1 + sizeof(bookkeep);
            (bookkeep_ptr + next)->end = mem.size;
            return mem.ptr + next;
        } else {
            perror("Not enough space.");
            exit(1);
        }
    }
    while (bookkeep_ptr->end < mem.size - 1) {
        if (bookkeep_ptr->end - bookkeep_ptr->start >= sz && bookkeep_ptr->free) {
            int next = bookkeep_ptr->start;
            (bookkeep_ptr + next)->start = bookkeep_ptr->end + 1 + sizeof(bookkeep);
            (bookkeep_ptr + next)->end = mem.size;
            return mem.ptr + next;
        }
        bookkeep_ptr = bookkeep_ptr + (bookkeep_ptr->end + 1);
    }

    perror("Failed to allocate space.");
    exit(1);
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    bookkeep *bookkeep_ptr = (bookkeep *) (ptr - sizeof(bookkeep));
    bookkeep *current = (bookkeep *) mem.ptr;
    bookkeep *previous = (bookkeep *) mem.ptr;
    while (current->start != bookkeep_ptr->start) {
        previous = current;
        current += current->end - current->start;
    }
    if (previous->free) {
        previous->end = current->end;
    }
    bookkeep *next = current + current->end - current->start;
    if (next->free) {
        current->end = next->end;
    }
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
