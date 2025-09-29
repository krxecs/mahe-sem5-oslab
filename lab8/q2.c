/*
 * Lab No. 8: IPC-3: DEADLOCK, LOCKING, SYNCHRONIZATION
 *
 * Question 2: Write a C program for the first readers-writers problem using
 * semaphores.
 *
 * Solution:
 * The first readers-writers problem prioritizes readers. A reader should not
 * be kept waiting unless a writer has already obtained permission to use the
 * shared object. This means no reader should wait for other readers to finish
 * simply because a writer is waiting.
 *
 * We use two semaphores and a counter:
 * 1. `mutex`: A binary semaphore to ensure mutual exclusion when updating
 * `read_count`. Initialized to 1.
 * 2. `wrt`: A binary semaphore used by both readers and writers to control
 * access to the shared resource. Initialized to 1.
 * 3. `read_count`: An integer that counts how many readers are currently
 * accessing the resource. Initialized to 0.
 *
 * Writer Logic:
 * A writer simply waits on the `wrt` semaphore, writes, and then signals `wrt`.
 * This ensures only one writer can be active at a time.
 *
 * Reader Logic:
 * The first reader to arrive waits on `wrt`, preventing any writers. Subsequent
 * readers can enter without waiting on `wrt`. The last reader to leave signals
 * `wrt`, allowing a waiting writer (if any) to proceed. The `mutex` semaphore
 * protects the `read_count` variable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

// Semaphores
sem_t mutex;
sem_t wrt;

// Shared data
int shared_data = 1;
int read_count = 0;

// Writer thread function
void *writer(void *arg) {
  long id = (long)arg;
  while (1) {
    printf("Writer %ld is trying to write.\n", id);

    // Wait for write access
    sem_wait(&wrt);

    // --- Critical Section ---
    printf("Writer %ld is writing...\n", id);
    shared_data *= 2;
    printf("Writer %ld wrote value: %d\n", id, shared_data);
    // --- End of Critical Section ---

    // Release write access
    sem_post(&wrt);

    printf("Writer %ld finished writing.\n", id);
    sleep(rand() % 3 + 1);
  }
  return NULL;
}

// Reader thread function
void *reader(void *arg) {
  long id = (long)arg;
  while (1) {
    printf("Reader %ld is trying to read.\n", id);

    // Entry section for readers
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1) {
      // First reader locks the resource for writers
      sem_wait(&wrt);
    }
    sem_post(&mutex);

    // --- Critical Section ---
    printf("Reader %ld is reading value: %d\n", id, shared_data);
    // --- End of Critical Section ---

    // Exit section for readers
    sem_wait(&mutex);
    read_count--;
    if (read_count == 0) {
      // Last reader unlocks the resource for writers
      sem_post(&wrt);
    }
    sem_post(&mutex);

    printf("Reader %ld finished reading.\n", id);
    sleep(rand() % 2 + 1);
  }
  return NULL;
}

int main(void) {
  pthread_t readers[NUM_READERS], writers[NUM_WRITERS];

  // Initialize semaphores
  sem_init(&mutex, 0, 1);
  sem_init(&wrt, 0, 1);

  // Create writer threads
  for (long i = 0; i < NUM_WRITERS; i++) {
    pthread_create(&writers[i], NULL, writer, (void *)(i + 1));
  }

  // Create reader threads
  for (long i = 0; i < NUM_READERS; i++) {
    pthread_create(&readers[i], NULL, reader, (void *)(i + 1));
  }

  // The program will run indefinitely. To stop it, use Ctrl+C.
  // We join the first writer thread just to prevent main from exiting.
  pthread_join(writers[0], NULL);

  // Destroy semaphores (this part is unreachable in this infinite loop)
  sem_destroy(&mutex);
  sem_destroy(&wrt);

  return 0;
}
