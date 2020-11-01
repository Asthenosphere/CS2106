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
    bookkeep_ptr->terminal = 1;

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

void print_memory(shmheap_memory_handle mem) {
    bookkeep *bookkeep_ptr = (bookkeep *) mem.ptr;
    int i = 0;
    while (i < 10) {
        printf("Block %d: %d %d %d\n", ++i, bookkeep_ptr->start, bookkeep_ptr->end, bookkeep_ptr->free);
        char *p = (char *) mem.ptr;
        p += bookkeep_ptr->end;
        bookkeep_ptr = (bookkeep *) p;
    }
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    //print_memory(mem);
    bookkeep *bookkeep_ptr = (bookkeep *) mem.ptr;
    while (bookkeep_ptr->end != mem.size) {
        if (bookkeep_ptr->free && (bookkeep_ptr->end - bookkeep_ptr->start >= sz)) {
            char *p = (char *) mem.ptr;
            p += bookkeep_ptr->end;

            if (bookkeep_ptr->end - bookkeep_ptr->start > sz + sizeof(bookkeep)) {
                bookkeep *next_seg = (bookkeep *) p;
                if (sz % 8 != 0) {
                    next_seg->start = (bookkeep_ptr->start + sz + 7 + sizeof(bookkeep)) / 8 * 8;
                } else {
                    next_seg->start = bookkeep_ptr->start + sz + sizeof(bookkeep);
                }
                next_seg->end = bookkeep_ptr->end;
                next_seg->free = 1;
                next_seg->terminal = 0;
            }
            bookkeep_ptr->free = 0;
            p = (char *) mem.ptr;
            return (void *) (p + bookkeep_ptr->start);
        } else {
            char *p = (char *) mem.ptr;
            p += bookkeep_ptr->end;
            bookkeep_ptr = (bookkeep *) p;
        }
    }

    if (bookkeep_ptr->end == mem.size) {
        if (bookkeep_ptr->end - bookkeep_ptr->start >= sz && bookkeep_ptr->free) {
            char *p = (char *) mem.ptr;
            if (sz % 8 != 0) {
                bookkeep_ptr->end = (bookkeep_ptr->start + sz + 7) / 8 * 8;
            } else {
                bookkeep_ptr->end = bookkeep_ptr->start + sz;
            }
            p += bookkeep_ptr->end;
            bookkeep_ptr->free = 0;

            bookkeep *next_seg = (bookkeep *) p;
            next_seg->start = bookkeep_ptr->end + sizeof(bookkeep);
            next_seg->end = mem.size;
            next_seg->free = 1;
            next_seg->terminal = 1;

            p = (char *) mem.ptr;
            p += bookkeep_ptr->start;
            return (void *) p;
        } else {
            perror("Not enough space.");
            exit(1);
        }
    }
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    char *p = (char *) ptr;
    p -= sizeof(bookkeep);
    bookkeep *bookkeep_ptr = (bookkeep *) p;
    bookkeep *current = (bookkeep *) mem.ptr;

    while (current->end + sizeof(bookkeep) != bookkeep_ptr->start) {
        //printf("Bookkeep: %d %d %d\n", bookkeep_ptr->start, bookkeep_ptr->end, bookkeep_ptr->free);
        //printf("Current: %d %d %d\n", current->start, current->end, current->free);
        char *tmp = (char *) mem.ptr;
        tmp += current->end;
        current = (bookkeep *) tmp;
    }

    if (current->free) {
        current->end = bookkeep_ptr->end;
    } else {
        char *tmp = (char *) mem.ptr;
        tmp += bookkeep_ptr->end;
        current = (bookkeep *) tmp;
        if (current->free) {
            bookkeep_ptr->end = current->end;
        }
    }

    bookkeep_ptr->free = 1;
    //print_memory(mem);
}

shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    shmheap_object_handle *handle = malloc(sizeof(shmheap_object_handle));

    handle->ptr = ptr;
    handle->shmheap_id = mem.shmheap_id;

    return *handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    return hdl.ptr;
}
