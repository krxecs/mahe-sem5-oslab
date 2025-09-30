/*
 * Lab 9, Exercise 3
 *
 * Question: Write a C program to generate prime numbers up to a given number
 * using threads. The parent thread will create a child thread and pass a number
 * as an argument to the child thread. The child thread will be responsible for
 * generating the prime numbers.
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "helper.h"

// Structure to pass range to the thread
typedef struct {
  int start;
  int end;
} prime_range;

// Function to check if a number is prime
bool is_prime(int n) {
  if (n <= 1) return false;
  for (int i = 2; i * i <= n; i++) {
    if (n % i == 0)
      return false;
  }
  return true;
}

// Thread function to find primes in a given range
void *find_primes(void *arg) {
  prime_range *range = (prime_range *)arg;
  int start = range->start, end = range->end;

  printf("Prime numbers between %d and %d are: ", start, end);
  for (int i = start; i <= end; i++) {
    if (is_prime(i))
      printf("%d ", i);
  }
  puts("");

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <start_number> <end_number>\n", argv[0]);
    return 1;
  }

  int start = atoi(argv[1]), end = atoi(argv[2]);

  if (start < 0 || end < 0 || start > end) {
    fprintf(stderr, "Invalid range. Please provide non-negative start and end numbers, with start <= end.\n");
    return 1;
  }

  pthread_t thread;
  prime_range range = {start, end};

  perrf(pthread_create(&thread, NULL, find_primes, &range) != 0, "Failed to create thread");
  perrf(pthread_join(thread, NULL) != 0, "Failed to join thread");

  return 0;
}
