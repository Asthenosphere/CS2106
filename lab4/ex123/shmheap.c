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
#include <semaphore.h>
#include <stdio.h>
#include "shmheap.h"

shmheap_memory_handle shmheap_create(const char *name, size_t len) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, len);

    void *ptr = mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    shmheap_memory_handle *handle = (shmheap_memory_handle *) ptr;
    handle->shmheap_id = shm_fd;
    handle->size = len;
    handle->ptr = ptr;
    char *p = (char *) ptr;
    p += sizeof(shmheap_memory_handle);
    shmheap_head * head = (shmheap_head *) p;
    sem_init(&(head->mutex), 1, 1);

    bookkeep *bookkeep_ptr = &(head->bookkeep_first);
    bookkeep_ptr->start = sizeof(shmheap_head) + sizeof(shmheap_memory_handle);
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

    shmheap_memory_handle *handle = (shmheap_memory_handle *) ptr;

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
    char *hdl = (char *) mem.ptr;
    hdl += sizeof(shmheap_memory_handle);
    shmheap_head * head = (shmheap_head *) hdl;
    sem_wait(&(head->mutex));
    char *tmp = (char *) hdl;
    tmp += sizeof(sem_t);
    bookkeep *bookkeep_ptr = (bookkeep *) tmp;
    while (bookkeep_ptr->end != mem.size) {
        if (bookkeep_ptr->free && (bookkeep_ptr->end - bookkeep_ptr->start >= sz)) {
            char *p = (char *) mem.ptr;
            if (sz % 8 != 0) {
                p += (bookkeep_ptr->start + sz + 7) / 8 * 8;
            } else {
                p += bookkeep_ptr->start + sz;
            }
            if (bookkeep_ptr->end - bookkeep_ptr->start > sz + sizeof(bookkeep)) {
                bookkeep *next_seg = (bookkeep *) p;
                if (sz % 8 != 0) {
                    next_seg->start = (bookkeep_ptr->start + sz + 7 + sizeof(bookkeep)) / 8 * 8;
                } else {
                    next_seg->start = bookkeep_ptr->start + sz + sizeof(bookkeep);
                }
                next_seg->end = bookkeep_ptr->end;
                bookkeep_ptr->end = next_seg->start - sizeof(bookkeep);
                next_seg->free = 1;
            }
            bookkeep_ptr->free = 0;
            p = (char *) mem.ptr;
            sem_post(&(head->mutex));
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

            p = (char *) mem.ptr;
            p += bookkeep_ptr->start;
            sem_post(&(head->mutex));
            return (void *) p;
        } else {
            perror("Not enough space.");
            exit(1);
        }
    }
    perror("Not enough space.");
    exit(1);
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    char *hdl = (char *) mem.ptr;
    hdl += sizeof(shmheap_memory_handle);
    shmheap_head * head = (shmheap_head *) hdl;
    if (sem_wait(&(head->mutex)) == -1) {
        fprintf(stderr, "Failed to lock semaphore\n");
        exit(1);
    }
    char *p_char = (char *) hdl;
    p_char += sizeof(sem_t);
    char *p = (char *) ptr;
    p -= sizeof(bookkeep);
    bookkeep *bookkeep_ptr = (bookkeep *) p;
    bookkeep *current = (bookkeep *) p_char;

    if (current->start == bookkeep_ptr->start) {
        char *tmp = (char *) mem.ptr;
        tmp += current->end;
        bookkeep *next_seg = (bookkeep *) tmp;
        if (next_seg->free) {
            current->end = next_seg->end;
        }
        current->free = 1;
        sem_post(&(head->mutex));
        return;
    }

    int count = 0;
    while (current->end + sizeof(bookkeep) != bookkeep_ptr->start) {
        if (count > 20) {
            exit(1);
        }
        char *tmp = (char *) mem.ptr;
        tmp += current->end;
        current = (bookkeep *) tmp;
        count++;
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
        bookkeep_ptr->free = 1;
        sem_post(&(head->mutex));
        return;
    }

    bookkeep_ptr->free = 1;
    if (sem_post(&(head->mutex)) == -1) {
        fprintf(stderr, "Failed to unlock mutex\n");
        exit(1);
    }
}

shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    shmheap_object_handle handle;

    handle.ptr = ptr;
    handle.shmheap_id = mem.shmheap_id;

    return handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    return hdl.ptr;
}
