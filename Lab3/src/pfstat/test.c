#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "pfstat.h"

int main(int argc, char **argv) {
  struct page_info *buf, *addspace;
  struct vma_struct *vma;
  int fd, t;
  buf = malloc(sizeof(struct page_info));
  fd = open("/dev/pfstat", O_RDWR);
  t = read(fd, buf, sizeof(struct page_info));

  printf("\n%ld physpages checked, total memory = %ld MB\n", buf->num_physpages,
         buf->num_physpages * 64 * 1024 / 1024 / 1024);
  printf("%ld valid pages, valid memory = %ld MB\n", buf->valid, buf->valid * 64 * 1024 / 1024 / 1024);

  printf("free: %d\n", buf->free);
  printf("locked: %d\n", buf->locked);
  printf("reserved: %d\n", buf->reserved);
  printf("swapcache: %d\n", buf->swapcache);
  printf("referenced: %d\n", buf->referenced);
  printf("slab: %d\n", buf->slab);
  printf("private: %d\n", buf->private);
  printf("uptodate: %d\n", buf->uptodate);
  printf("dirty: %d\n", buf->dirty);
  printf("active: %d\n", buf->active);
  printf("writeback: %d\n", buf->writeback);
  printf("mappedtodisk: %d\n", buf->mappedtodisk);

  free(buf);
  close(fd);
}
