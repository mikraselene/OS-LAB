#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "vma.h"

int global = 999;

void print_mm(struct buffer *addr_space) {
  printf(
      "task_size: %lu, "
      "highest_vm_end: %lu, "
      "mm_users: %d, "
      "mm_count: %d\n",
      addr_space->task_size, addr_space->highest_vm_end, addr_space->mm_users, addr_space->mm_count);
  printf(
      "map_count: %d, "
      "total_vm: %ld, "
      "locked_vm: %ld, "
      "pinned_vm: %ld\n",
      addr_space->map_count, addr_space->total_vm, addr_space->locked_vm, addr_space->pinned_vm);
  printf(
      "data_vm: %ld, "
      "exec_vm: %ld, "
      "stack_vm: %ld\n",
      addr_space->data_vm, addr_space->exec_vm, addr_space->stack_vm);
  printf(
      "start_code: 0x%lx, "
      "end_code: 0x%lx, "
      "start_data: 0x%lx, "
      "end_data: 0x%lx\n",
      addr_space->start_code, addr_space->end_code, addr_space->start_data, addr_space->end_data);
  printf(
      "start_brk: 0x%lx, "
      "brk: 0x%lx, "
      "start_stack: 0x%lx\n",
      addr_space->start_brk, addr_space->brk, addr_space->start_stack);
  printf(
      "arg_start: 0x%lx, "
      "arg_end: 0x%lx, "
      "env_start: 0x%lx, "
      "env_end: 0x%lx\n",
      addr_space->arg_start, addr_space->arg_end, addr_space->env_start, addr_space->env_end);
}

void print_each_vma(struct vma_struct *vma, int count) {
  int i;
  for (i = 0; i < count; i++) {
    printf(
        "start: 0x%lx, "
        "end: 0x%lx, "
        "flags: 0x%lx, "
        "filename: %s\n",
        vma->vm_start, vma->vm_end, vma->vm_flags, vma->filename);
    vma++;
  }
  printf("address of i is 0x%p\n", &i);
}

int main(int argc, char **argv) {
  static int static_int = 999;
  int fd1 = open("./vma.o", O_RDWR);

  void *ptr = mmap(NULL, 40000, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  struct buffer *buf = (struct buffer *)malloc(sizeof(struct buffer));
  int fd = open("/dev/vma", O_RDWR);
  printf("file opened.\n");
  int t = read(fd, buf, sizeof(struct buffer));
  printf("t = %d, map_count: %d\n", t, buf->map_count);
  int datasize = (buf->map_count) * sizeof(struct vma_struct) + sizeof(struct buffer);
  free(buf);
  printf("datasize = %d\n", datasize);
  buf = (struct buffer *)malloc(datasize);
  struct buffer *addr_space = buf;
  buf++;
  struct vma_struct *vma = (struct vma_struct *)buf;
  t = read(fd, addr_space, datasize);
  print_mm(addr_space);
  printf("addr_space print ok.\n");

  print_each_vma(vma, addr_space->map_count);
  free(addr_space);
  close(fd);
  printf("start of print_mm: %p\n", print_mm);
  printf("global val: %p\n", &global);
  printf("local val: %p\n", &t);
  printf("static val: %p\n", &static_int);
  printf("malloc alocated mem: %p\n", buf);
}
