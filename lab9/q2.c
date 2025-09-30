/*
 * Lab 9, Exercise 2
 *
 * Question: Write a C program to find the summation of a non-negative integer
 * using a separate thread.
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"

// Function to calculate the sum of non-negative integers
void* sum_runner(void* param) {
  long long limit = (long long)param;
  long long* sum = malloc(sizeof(long long));
  if (!sum) {
    perror("Failed to allocate memory for sum");
    return NULL;
  }

  *sum = 0;
  for (long long i = 0; i <= limit; i++)
    *sum += i;

  return sum;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <positive_integer>\n", argv[0]);
    return 1;
  }

  long long limit = atoll(argv[1]);
  if (limit < 0) {
    fprintf(stderr, "Please enter a non-negative integer.\n");
    return 1;
  }

  pthread_t thread_id;
  long long* result;

  perrf(pthread_create(&thread_id, NULL, sum_runner, (void*)limit) != 0, "Failed to create thread");
  perrf(pthread_join(thread_id, (void**)&result) != 0, "Failed to join thread");

  if (result)
    printf("The sum of integers up to %lld is %lld\n", limit, *result);

  free(result);
  return 0;
}
