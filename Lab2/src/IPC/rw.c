#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./xsem.h"

#define NUM_WRITER 5
#define NUM_READER 10
#define ITERATION_TIME 5  // (second)

pthread_mutex_t write_mutex, read_mutex;
semaphore_t *read_try, *resource;

int thread_id[NUM_WRITER + NUM_READER];
int write_cnt = 0, read_cnt = 0;
bool interrupt = false;
unsigned int seed = 123456;

void *reading(void *id) {
  while (!interrupt) {
    sem_wait(read_try->sem);          // P(reader): a reader is waiting
    pthread_mutex_lock(&read_mutex);  // P(read_mutex): lock entry
    read_cnt++;
    if (read_cnt == 1) {        // this reader is the first
      sem_wait(resource->sem);  // P(resource): lock the resource
    }
    pthread_mutex_unlock(&read_mutex);  // V(read_mutex): release entry
    sem_post(read_try->sem);            // V(reader): a reader is ok

    printf("%d <- Resource (READ)\n", *((int *)id));  // do reading

    pthread_mutex_lock(&read_mutex);  // P(read_mutex): lock exit
    read_cnt--;
    if (read_cnt == 0) {        // this reader is the last
      sem_post(resource->sem);  // V(resource): release the resourse
    }
    pthread_mutex_unlock(&read_mutex);  // V(read_mutex) release exit

    usleep(rand_r(&seed) % 200000);
  }
  return NULL;
}

void *writing(void *id) {
  while (!interrupt) {
    pthread_mutex_lock(&write_mutex);  // P(write_mutex): lock entry
    write_cnt++;
    if (write_cnt == 1) {       // this writer is the first
      sem_wait(read_try->sem);  // P(read_try): prevent readers from trying
    }
    pthread_mutex_unlock(&write_mutex);  // V(write_mutex): release entry

    sem_wait(resource->sem);  // P(resource): reserve the resource for yourself

    printf("%d -> Resource (WRITE)\n", *(int *)id);  // do writing

    sem_post(resource->sem);  // V(resource): release the resource

    pthread_mutex_lock(&write_mutex);  // P(write_mutex): lock exit
    write_cnt--;
    if (write_cnt == 0) {       // this writer is the last
      sem_post(read_try->sem);  // V(read_try): allow readers to read
    }
    pthread_mutex_unlock(&write_mutex);  // V(write_mutex): release exit

    usleep(rand_r(&seed) % 200000);
  }
  return NULL;
}

int main() {
  seed = time(NULL);

  pthread_t read[10], write[5];

  pthread_t thread_pool[NUM_WRITER + NUM_READER];
  pthread_mutex_init(&write_mutex, NULL);
  pthread_mutex_init(&read_mutex, NULL);

  resource = semaphore_init("/resource", 1);
  read_try = semaphore_init("/read_try", 1);

  for (int i = 0; i < NUM_WRITER + NUM_READER; i++) {
    thread_id[i] = i;
    pthread_create(&thread_pool[i], NULL, i < NUM_WRITER ? writing : reading,
                   (void *)&thread_id[i]);
  }

  for (int i = 0; i < NUM_WRITER + NUM_READER; i++) {
    pthread_join(thread_pool[i], NULL);
  }

  semaphore_clear(resource);
  semaphore_clear(read_try);
  pthread_mutex_destroy(&write_mutex);
  pthread_mutex_destroy(&read_mutex);
}
