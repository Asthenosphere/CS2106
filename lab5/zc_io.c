#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "zc_io.h"

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
  // Insert the fields you need here.
  char * ptr;
  size_t size;
  size_t offset;
  int fd;
  sem_t mutex;
  sem_t roomEmpty;
  int nReader;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
  struct zc_file * file = malloc(sizeof(struct zc_file));
  file->fd = open(path, O_RDWR | O_CREAT, 0666);
  if (file->fd < 0) {
      fprintf(stderr, "Error opening file");
      exit(1);
  }
  struct stat st;
  fstat(file->fd, &st);
  char * addr;
  if (st.st_size == 0) {
      addr = mmap(NULL, 1, PROT_WRITE | PROT_READ, MAP_SHARED, file->fd, 0);
  } else {
      addr = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, file->fd, 0);
  }
  file->ptr = addr;
  file->size = st.st_size;
  file->offset = 0;
  sem_init(&(file->mutex), 0, 1);
  sem_init(&(file->roomEmpty), 0, 1);
  file->nReader = 0;
  return file;
}

int zc_close(zc_file *file) {
  int flag = munmap(file->ptr, file->size);
  if (flag < 0) {
      return flag;
  }
  close(file->fd);
  sem_destroy(file->mutex);
  sem_destroy(file->roomEmpty);
  free(file);
  return 0;
}

const char *zc_read_start(zc_file *file, size_t *size) {
  sem_wait(file->mutex);
  file->nReader++;
  if (file->nReader == 1) {
    sem_wait(file->roomEmpty);
  }
  sem_post(file->mutex);
  char *res = file->ptr;
  if (*size > file->size - file->offset) {
      *size = file->size - file->offset;
  }
  res += file->offset;
  file->offset += *size;
  return res;
}

void zc_read_end(zc_file *file) {
  sem_wait(file->mutex);
  file->nReader--;
  if (file->nReader == 0) {
    sem_post(file->roomEmpty);
  }
  sem_post(file->mutex);
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
  sem_wait(file->roomEmpty);
  if (size + file->offset > file->size) {
    if (ftruncate(file->fd, file->offset + size) < 0) {
      fprintf(stderr, "Error truncating file");
      exit(1);
    }
    void *new_addr = mremap(file->ptr, file->size, file->offset + size, MREMAP_MAYMOVE);
    if (new_addr == MAP_FAILED) {
      fprintf(stderr, "Error remapping file");
      exit(1);
    }
    file->ptr = (char *) new_addr;
    file->size = file->offset + size;
  }

  char * addr = file->ptr;
  printf("File size: %zu Size: %zu Offset: %zu\n", file->size, size, file->offset);
  addr += file->offset;
  file->offset += size;
  printf("File size: %zu Size: %zu Offset: %zu\n", file->size, size, file->offset);
  return addr;
}

void zc_write_end(zc_file *file) {
  msync(file->ptr, file->size, MS_SYNC);
  sem_post(file->roomEmpty);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
  sem_wait(file->roomEmpty);
  switch (whence) {
    case SEEK_SET:
      file->offset = offset;
      break;
    case SEEK_CUR:
      file->offset += offset;
      break;
    case SEEK_END:
      file->offset = file->size + offset;
      break;
    default:
      sem_post(file->roomEmpty);
      return -1;
  }
  sem_post(file->roomEmpty);
  return file->offset;
}

/**************
 * Exercise 5 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
  // To implement
  return -1;
}
