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

#include "proc_relate.h"

int my_thread() {
  int fd, t;
  char *buf;
  float x;
  printf("In thread, pid = %d, tid = %d, thread id = %ld\n", getpid(), syscall(__NR_gettid), pthread_self());
  pthread_yield();
  sleep(10);
  for (int i = 0; i < 10000; i++) {
    x = sin(i);
  }
  printf("thread has ended.\n");
  // pause();
}

void printdata(struct proc_info *buf, int t) {
  printf("[%d]\n", t);
  for (int i = 0; i < t; i++) {
    printf("-------------------------------------------------------\n");
    printf("pid = %d\ttgid = %d\tcomm = %s\tsessionid = %d\n", buf->pid, buf->tgid, buf->comm, buf->sessionid);
    printf("mm = %p\tactive_mm = %p\n", buf->mm, buf->active_mm);
    printf("parent = %d\treal_parent = %d\tgroup_leader = %d\n", buf->parent, buf->real_parent, buf->group_leader);
    buf++;
  }
  printf("-------------------------------------------------------\n");
}

int main(int argc, char **argv) {
  struct proc_info *buf, *addspace;
  struct vma_struct *vma;
  int fd, t;
  pthread_t tid;
  if (fork() == 0) {
    if (fork() == 0) {
      printf("this is another child, pid is %d\n", getpid());
      pthread_create(&tid, NULL, my_thread, NULL);
      pause();
    } else {
      fork();
      fork();
      printf("this is a child, pid is %d\n", getpid());
      pause();
    }
  }
  printf("here is parent process, pid = %d\n", getpid());
  buf = (struct proc_info *)malloc(sizeof(struct proc_info) * 30);
  fd = open("/dev/proc_relate", O_RDWR);
  t = read(fd, buf, sizeof(struct proc_info) * 30);
  t = t / sizeof(struct proc_info);
  printdata(buf, t);
  free(buf);
  close(fd);
}
