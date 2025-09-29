/*
 * Lab No. 8: IPC-3: DEADLOCK, LOCKING, SYNCHRONIZATION
 *
 * Question 3: Write a Code to access a shared resource which causes deadlock
 * using improper use of semaphore.
 *
 * Solution:
 * Deadlock can occur when four conditions are met simultaneously:
 * 1. Mutual Exclusion
 * 2. Hold and Wait
 * 3. No Preemption
 * 4. Circular Wait
 *
 * This program demonstrates a deadlock by violating the principle of ordering
 * resource requests, which leads to a circular wait condition.
 *
 * We have two threads (Thread 1 and Thread 2) and two resources represented
 * by semaphores (resource_A and resource_B).
 *
 * - Thread 1 locks resource_A, then tries to lock resource_B.
 * - Thread 2 locks resource_B, then tries to lock resource_A.
 *
 * If Thread 1 acquires resource_A and Thread 2 acquires resource_B concurrently,
 * a deadlock will occur. Thread 1 will wait indefinitely for resource_B (held by
 * Thread 2), and Thread 2 will wait indefinitely for resource_A (held by Thread 1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Two resources represented by semaphores (acting as mutexes)
sem_t resource_A;
sem_t resource_B;

// Thread 1 function: Locks A, then B
void *thread_1_func(void *arg) {
    printf("Thread 1 trying to lock resource A...\n");
    sem_wait(&resource_A);
    printf("Thread 1 locked resource A.\n");

    // Introduce a small delay to make deadlock more likely
    sleep(1);

    printf("Thread 1 trying to lock resource B...\n");
    sem_wait(&resource_B);
    printf("Thread 1 locked resource B.\n");

    // --- Critical Section ---
    printf("Thread 1 is in its critical section.\n");
    // --- End of Critical Section ---

    sem_post(&resource_B);
    printf("Thread 1 unlocked resource B.\n");
    sem_post(&resource_A);
    printf("Thread 1 unlocked resource A.\n");

    return NULL;
}

// Thread 2 function: Locks B, then A
void *thread_2_func(void *arg) {
    printf("Thread 2 trying to lock resource B...\n");
    sem_wait(&resource_B);
    printf("Thread 2 locked resource B.\n");

    // Introduce a small delay
    sleep(1);

    printf("Thread 2 trying to lock resource A...\n");
    sem_wait(&resource_A);
    printf("Thread 2 locked resource A.\n");

    // --- Critical Section ---
    printf("Thread 2 is in its critical section.\n");
    // --- End of Critical Section ---

    sem_post(&resource_A);
    printf("Thread 2 unlocked resource A.\n");
    sem_post(&resource_B);
    printf("Thread 2 unlocked resource B.\n");

    return NULL;
}

int main(void) {
    pthread_t thread1, thread2;

    // Initialize semaphores (as binary semaphores/mutexes)
    sem_init(&resource_A, 0, 1);
    sem_init(&resource_B, 0, 1);

    printf("Starting deadlock simulation...\n");

    // Create the two threads
    pthread_create(&thread1, NULL, thread_1_func, NULL);
    pthread_create(&thread2, NULL, thread_2_func, NULL);

    // Wait for the threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Simulation finished. If you see this, deadlock was avoided!\n");
    printf("If the program hangs, a deadlock occurred as expected.\n");

    sem_destroy(&resource_A);
    sem_destroy(&resource_B);

    return 0;
}
