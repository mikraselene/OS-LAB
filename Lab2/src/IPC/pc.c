#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./xsem.h"

#define BUFFER_SIZE 5
#define NUM_PRODUCER 2
#define NUM_CONSUMER 3
#define ITERATION_TIME 1  // (second)

pthread_mutex_t mutex;
semaphore_t *empty, *full;

typedef int item;
item buffer[BUFFER_SIZE];
int buffer_index = 0;
int upper_cnt = 0, lower_cnt = 0;  // stock count
bool interrupt = false;
unsigned int seed = 123456;

void *producing(void *id) {
  while (!interrupt) {
    usleep(rand_r(&seed) % 100000);

    sem_wait(empty->sem);        // P(empty): if empty, produce items
    pthread_mutex_lock(&mutex);  // P(mutex)

    item cur_item = rand_r(&seed) % 2;  // produce 'A' or 'a'
    buffer[buffer_index] = cur_item;
    buffer_index++;

    cur_item ? upper_cnt++ : lower_cnt++;
    printf("  %d -> %c  ", *(int *)id, cur_item ? 'A' : 'a');
    printf("            ");
    printf("   A(%d), a(%d) \n", upper_cnt, lower_cnt);

    pthread_mutex_unlock(&mutex);  // V(mutex)
    sem_post(full->sem);           // V(full): the buffer is full
  }
  return NULL;
}

void *consuming(void *id) {
  while (!interrupt) {
    usleep(rand_r(&seed) % 300000);

    sem_wait(full->sem);         // P(full): if full, consume items
    pthread_mutex_lock(&mutex);  // P(mutex)

    buffer_index--;
    item cur_item = buffer[buffer_index];

    cur_item ? upper_cnt-- : lower_cnt--;
    printf("            ");
    printf("  %d <- %c  ", *(int *)id, cur_item ? 'A' : 'a');
    printf("   A(%d), a(%d) \n", upper_cnt, lower_cnt);

    pthread_mutex_unlock(&mutex);  // V(mutex)
    sem_post(empty->sem);          // V(empty): the buffer is empty
  }
  return NULL;
}

int main() {
  seed = time(NULL);

  int thread_id[NUM_PRODUCER + NUM_CONSUMER];
  pthread_t thread_pool[NUM_PRODUCER + NUM_CONSUMER];
  pthread_mutex_init(&mutex, NULL);

  empty = semaphore_init("/empty", BUFFER_SIZE);
  full = semaphore_init("/full", 0);

  for (int i = 0; i < NUM_PRODUCER + NUM_CONSUMER; i++) {
    thread_id[i] = i;
    pthread_create(&thread_pool[i], NULL,
                   i < NUM_PRODUCER ? producing : consuming,
                   (void *)&thread_id[i]);
  }

  printf("------------------------------------\n");
  printf(" PRODUCER    CONSUMER      STOCK    \n");
  printf("------------------------------------\n");

  sleep(ITERATION_TIME);
  interrupt = true;

  for (int i = 0; i < NUM_PRODUCER + NUM_CONSUMER; i++) {
    pthread_join(thread_pool[i], NULL);
  }

  printf("------------------------------------\n");
  printf("OK.\n");

  semaphore_clear(empty);
  semaphore_clear(full);
  pthread_mutex_destroy(&mutex);
}
