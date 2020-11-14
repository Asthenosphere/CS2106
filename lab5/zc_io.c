#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
  struct zc_file * file = malloc(sizeof(struct zc_file));
  file->fd = open(path, O_RDWR | O_CREAT);
  if (file->fd < 0) {
      fprintf(stderr, "Error opening file");
      exit(1);
  }
  struct stat st;
  fstat(file->fd, &st);
  char * addr;
  if (st.st_size == 0) {
      addr = mmap(NULL, 1, PROT_WRITE | PROT_READ, MAP_PRIVATE, file->fd, 0);
  } else {
      addr = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, file->fd, 0);
  }
  file->ptr = addr;
  file->size = st.st_size;
  file->offset = 0;
  return file;
}

int zc_close(zc_file *file) {
  int flag = munmap(file->ptr, file->size);
  if (flag < 0) {
      return flag;
  }
  free(file);
  return 0;
}

const char *zc_read_start(zc_file *file, size_t *size) {
  char *res = file->ptr;
  if (*size > file->size - file->offset) {
      *size = file->size - file->offset;
  }
  res += file->offset;
  file->offset += *size;
  return res;
}

void zc_read_end(zc_file *file) {
  // To implement
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
  char * addr;
  if (size + file->offset > file->size) {
    file->ptr = mremap(file->ptr, file->size, file->offset + size, MREMAP_MAYMOVE);
    file->size = file->offset + size;
    addr = file->ptr;
  } else {
    addr = file->ptr;
  }
  printf("File size: %zu Size: %zu Offset: %zu\n", file->size, size, file->offset);
  addr += file->offset;
  file->offset += size;
  return addr;
}

void zc_write_end(zc_file *file) {
  // To implement
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
  // To implement
  return -1;
}

/**************
 * Exercise 5 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
  // To implement
  return -1;
}
