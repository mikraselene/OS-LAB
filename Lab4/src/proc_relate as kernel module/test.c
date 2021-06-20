#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "sysx.h"

#define IS_KERNEL_MODULE 1

void my_thread() {
  printf("This is my thread, my process ID = %d, thread ID = %ld\n", getpid(), pthread_self());
  sleep(2);
}

int main() {
  char *buf = malloc(4096);
  pthread_t threadx;
  int t = pthread_create(&threadx, NULL, (void *)my_thread, NULL);
  if (!fork()) {
    printf("This is the first process, and my ID = %d\n", getpid());
    sleep(3);
    exit(0);
  }
  if (!fork()) {
    printf("This is the second process, and my ID = %d\n", getpid());
    sleep(3);
    exit(0);
  }
  printf("Here is me, and my ID = %d\n", getpid());
  sleep(1);
  memset(buf, 0, 176);
#if IS_KERNEL_MODULE
  int fd = open("/dev/sysx", O_RDWR);
  int res = read(fd, buf, 4096);
#else
  int res = syscall(294, buf);
#endif
  struct buffer *b = (struct buffer *)buf;
  int count = b->count;
  printf("count = %d\n", count);
  struct proc_info *p_info = (struct proc_info *)(buf + sizeof(int));
  for (int i = 0; i < count; i++) {
    printf("pid = %d, comm = %s\n", p_info->pid, p_info->comm);
    p_info++;
  }
  printf("read result = %d\n", res);
  free(buf);
}
