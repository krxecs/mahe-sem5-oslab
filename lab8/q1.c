/*
 * Lab No. 8: IPC-3: DEADLOCK, LOCKING, SYNCHRONIZATION
 *
 * Question 1: Modify the Producer-Consumer program from the lab manual so that,
 * a producer can produce at the most 10 items more than what the consumer has
 * consumed.
 *
 * Solution:
 * This problem is a classic bounded-buffer scenario. The condition that the
 * producer can produce at most 10 items more than the consumer means that the
 * buffer size should be 10. (items_in_buffer = items_produced - items_consumed).
 *
 * We use three semaphores:
 * 1. `mutex`: A binary semaphore for mutual exclusion to access the buffer.
 * Initialized to 1.
 * 2. `empty`: A counting semaphore to track the number of empty slots in the
 * buffer. Initialized to BUFFER_SIZE (10). The producer waits on this.
 * 3. `full`: A counting semaphore to track the number of filled slots in the
 * buffer. Initialized to 0. The consumer waits on this.
 *
 * The program creates one producer thread and one consumer thread that run
 * concurrently for 20 items.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "helper.h"

#define BUFFER_SIZE 10
#define MAX_ITEMS 20

// Shared buffer
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

// Semaphores
sem_t mutex;
sem_t empty;
sem_t full;

// Producer thread function
void *produce(void *arg) {
  for (int i = 0; i < MAX_ITEMS; i++) {
    // Wait for an empty slot
    sem_wait(&empty);

    // Acquire lock
    sem_wait(&mutex);

    // Produce an item and add it to the buffer
    buffer[in] = i;
    printf("P: %d at index %d\n", buffer[in], in);
    in = (in + 1) % BUFFER_SIZE;

    // Release lock
    sem_post(&mutex);

    // Signal that a slot is now full
    sem_post(&full);

    // Simulate some work
    sleep(rand() % 2);
  }
  pthread_exit(NULL);
}

// Consumer thread function
void *consume(void *arg) {
  for (int i = 0; i < MAX_ITEMS; i++) {
    // Wait for a full slot
    sem_wait(&full);

    // Acquire lock
    sem_wait(&mutex);

    // Consume an item from the buffer
    int item = buffer[out];
    printf("C: %d from index %d\n", item, out);
    out = (out + 1) % BUFFER_SIZE;

    // Release lock
    sem_post(&mutex);

    // Signal that a slot is now empty
    sem_post(&empty);

    // Simulate some work
    sleep(rand() % 3);
  }
  pthread_exit(NULL);
}

int main(void) {
  // Initialize threads
  pthread_t producer_thread, consumer_thread;

  // Initialize semaphores
  sem_init(&mutex, 0, 1);
  sem_init(&empty, 0, BUFFER_SIZE); // Buffer is initially empty
  sem_init(&full, 0, 0);      // Buffer is initially not full

  perrf(pthread_create(&producer_thread, NULL, produce, NULL) != 0,
      "Failed to create producer thread");
  perrf(pthread_create(&consumer_thread, NULL, consume, NULL) != 0,
      "Failed to create consumer thread");

  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);

  puts("Simulation finished.");

  sem_destroy(&mutex);
  sem_destroy(&empty);
  sem_destroy(&full);

  return 0;
}
