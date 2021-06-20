#ifndef LAB2_SRC_IPC_XSEM_H_
#define LAB2_SRC_IPC_XSEM_H_

#include <semaphore.h>
#include <stdio.h>

typedef struct semaphore {
  sem_t *sem;
  char name[20];
} semaphore_t;

semaphore_t *semaphore_init(char name[], int size) {
  sem_t *ptr;
  ptr = sem_open(name, O_CREAT, 0644, size);
  semaphore_t *ret = malloc(sizeof(semaphore_t));
  ret->sem = ptr;
  snprintf(ret->name, sizeof(ret->name), "%s", name);
  return ret;
}

void semaphore_clear(semaphore_t *s) {
  sem_close(s->sem);
  sem_unlink(s->name);
  free(s);
}

#endif  // LAB2_SRC_IPC_XSEM_H_
