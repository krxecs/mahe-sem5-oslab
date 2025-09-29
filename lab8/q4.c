/*
 * Lab No. 8: IPC-3: DEADLOCK, LOCKING, SYNCHRONIZATION
 *
 * Question 4: Write a program using semaphore to demonstrate the working of
 * the sleeping barber problem.
 *
 * Solution:
 * This is a classic synchronization problem. We have one barber, one barber
 * chair, and a number of waiting chairs for customers.
 *
 * - If there are no customers, the barber goes to sleep.
 * - When a customer arrives, they wake up the barber if he is sleeping.
 * - If the barber is busy, the customer waits in a waiting room chair.
 * - If all waiting chairs are full, the customer leaves.
 *
 * We use three semaphores and a shared variable:
 * 1. `customers`: Counts waiting customers. The barber waits on this.
 * Initialized to 0.
 * 2. `barber`: Signals that the barber is ready to cut hair. Customers wait
 * on this. Initialized to 0.
 * 3. `mutex`: Provides mutual exclusion for accessing the `waiting_customers`
 * count. Initialized to 1.
 * 4. `waiting_customers`: An integer counting customers in the waiting room.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define WAITING_CHAIRS 5
#define NUM_CUSTOMERS 15

// Semaphores
sem_t customers; // Number of customers waiting for service
sem_t barber;  // Barber is ready to cut hair
sem_t mutex;   // For mutual exclusion

// Shared variable
int waiting_customers = 0;

// Barber thread function
void *barber_func(void *arg) {
  printf("Barber has opened the shop.\n");
  while (1) {
    // Wait for a customer (sleeps if no customers)
    printf("Barber is sleeping...\n");
    sem_wait(&customers);

    // A customer has arrived, acquire mutex to modify waiting_customers
    sem_wait(&mutex);

    // One less customer in the waiting room
    waiting_customers--;
    printf("Barber woke up. Waiting customers: %d\n", waiting_customers);

    // Signal that the barber is ready to cut hair
    sem_post(&barber);

    // Release mutex
    sem_post(&mutex);

    // --- Cut Hair ---
    printf("Barber is cutting hair...\n");
    sleep(3); // Simulate haircut time
    printf("Barber has finished cutting hair.\n");
  }
  return NULL;
}

// Customer thread function
void *customer_func(void *arg) {
  long id = (long)arg;

  sleep(rand() % 5); // Customer arrives at a random time
  printf("Customer %ld arrived.\n", id);

  // Acquire mutex to check/modify waiting_customers count
  sem_wait(&mutex);

  if (waiting_customers < WAITING_CHAIRS) {
    // There is a free chair in the waiting room
    waiting_customers++;
    printf("Customer %ld is waiting. Waiting customers: %d\n", id, waiting_customers);

    // Signal/wake up the barber
    sem_post(&customers);

    // Release mutex
    sem_post(&mutex);

    // Wait for the barber to be ready
    sem_wait(&barber);

    // --- Get Haircut ---
    printf("Customer %ld is getting a haircut.\n", id);
  } else {
    // No free chairs, customer leaves
    sem_post(&mutex);
    printf("Waiting room full, Customer %ld is leaving.\n", id);
  }

  return NULL;
}

int main(void) {
  pthread_t barber_thread;
  pthread_t customer_threads[NUM_CUSTOMERS];

  // Initialize semaphores
  sem_init(&customers, 0, 0);
  sem_init(&barber, 0, 0);
  sem_init(&mutex, 0, 1);

  // Create the barber thread
  pthread_create(&barber_thread, NULL, barber_func, NULL);

  // Create customer threads
  for (long i = 0; i < NUM_CUSTOMERS; i++) {
    pthread_create(&customer_threads[i], NULL, customer_func, (void *)(i + 1));
  }

  // Wait for all customers to finish (either get a haircut or leave)
  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    pthread_join(customer_threads[i], NULL);
  }

  // The barber runs forever, so we'll just exit here for the simulation.
  // In a real scenario, you might have a condition to close the shop.
  printf("All customers have been served or have left. Closing shop.\n");

  // We can cancel the barber thread to exit cleanly
  pthread_cancel(barber_thread);

  // Destroy semaphores
  sem_destroy(&customers);
  sem_destroy(&barber);
  sem_destroy(&mutex);

  return 0;
}
