#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./xsem.h"

#define NUM_SEAT 5
#define NUM_CUSTOMER 10

pthread_mutex_t mutex;
semaphore_t *barber_ready, *customer_ready, *seat_ready;

int thread_id[NUM_CUSTOMER];
int available_seats_cnt = NUM_SEAT, unlucky_customer_cnt = 0;
bool interrupt = false;
unsigned int seed = 123456;

void *barbering(void *id) {
  int served_customer_cnt = 0;
  while (!interrupt) {
    sem_wait(customer_ready->sem);  // P(customer_ready): waiting for customers
    sem_wait(seat_ready->sem);      // P(seat_ready): waiting for a seat
    available_seats_cnt++;          // occupy a seat
    sem_post(seat_ready->sem);      // V(seat_ready): seat is ready

    pthread_mutex_lock(&mutex);               // P(mutex)
    usleep(rand_r(&seed) % 200000 + 300000);  // barbering (300 ms ~ 500 ms)
    pthread_mutex_unlock(&mutex);             // V(mutex)

    sem_post(barber_ready->sem);  // V(barber_ready): barber is ready
    served_customer_cnt++;
    if (served_customer_cnt + unlucky_customer_cnt == NUM_CUSTOMER) {
      interrupt = true;
    }
  }
  return NULL;
}

void *being_served(void *id) {
  sem_wait(seat_ready->sem);  // P(seat_ready): waiting for a seat
  if (available_seats_cnt > 0) {
    available_seats_cnt--;  // occupy a seat

    printf("Customer %d is waiting...  ", *(int *)id);
    printf("Avl = %d\n", available_seats_cnt);

    sem_post(customer_ready->sem);  // V(customer_ready): customer is ready
    sem_post(seat_ready->sem);      // V(seat_ready): get the seat
    sem_wait(barber_ready->sem);    // P(barber_ready): waiting for a barbar

    printf("Customer %d is OK.         ", *(int *)id);
    printf("Avl = %d\n", available_seats_cnt);
  } else {
    sem_post(seat_ready->sem);  // V(seat_ready): do not wait any more
    unlucky_customer_cnt++;     // a customer left
    printf("Customer %d left. \n", *(int *)id);
  }
  return NULL;
}

void *spawn() {
  int cnt = 0;
  pthread_t thread_pool[NUM_CUSTOMER];

  for (int i = 0; i < NUM_CUSTOMER; i++) {
    thread_id[i] = i;
    pthread_create(&thread_pool[i], NULL, being_served, (void *)&thread_id[i]);
    usleep(rand_r(&seed) % 100000 + 50000);  // spawning (50 ms ~ 150 ms)
  }
  return NULL;
}

int main() {
  seed = time(NULL);
  pthread_t barber;
  pthread_t customer_spawner;
  pthread_mutex_init(&mutex, NULL);

  customer_ready = semaphore_init("/customer_ready", 0);
  barber_ready = semaphore_init("/barber_ready", 0);
  seat_ready = semaphore_init("/seat_ready", 1);

  pthread_create(&barber, NULL, barbering, NULL);
  pthread_create(&customer_spawner, NULL, spawn, NULL);

  pthread_join(barber, NULL);
  pthread_join(customer_spawner, NULL);

  semaphore_clear(customer_ready);
  semaphore_clear(barber_ready);
  semaphore_clear(seat_ready);

  pthread_mutex_destroy(&mutex);
}
