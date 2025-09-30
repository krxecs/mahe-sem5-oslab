/*
 * Lab 9, Exercise 4
 *
 * Question: Write a C program that creates two threads to find the sum of
 * even and odd numbers from a given set of numbers.
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "helper.h"

// Data structure to pass array info to threads
typedef struct {
  int *array;
  int size;
  int odd_sum;
  int even_sum;
} array_info;

// Thread function to sum even numbers
void *sum_even(void *arg) {
  array_info *info = (array_info *)arg;

  info->even_sum = 0;
  for (int i = 0; i < info->size; i++) {
    if (info->array[i] % 2 == 0)
      info->even_sum += info->array[i];
  }

  return NULL;
}

// Thread function to sum odd numbers
void *sum_odd(void *arg) {
  array_info *info = (array_info *)arg;

  info->odd_sum = 0;
  for (int i = 0; i < info->size; i++) {
    if (info->array[i] % 2 != 0)
      info->odd_sum += info->array[i];
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <num1> <num2> ... <numN>\n", argv[0]);
    return 1;
  }

  int size = argc - 1;
  int *numbers = malloc(size * sizeof numbers[0]);
  perrf(!numbers, "Failed to allocate memory for numbers");

  for (int i = 0; i < size; i++)
    numbers[i] = atoi(argv[i + 1]);

  array_info info = {numbers, size};
  pthread_t even_thread, odd_thread;

  perrf_free(pthread_create(&even_thread, NULL, sum_even, &info) != 0, numbers, "Failed to create even_thread");
  perrf_free(pthread_create(&odd_thread, NULL, sum_odd, &info) != 0, numbers, "Failed to create odd_thread");

  perrf_free(pthread_join(even_thread, NULL) != 0, numbers, "Failed to join even_thread");
  perrf_free(pthread_join(odd_thread, NULL) != 0, numbers, "Failed to join odd_thread");

  puts("Sum of");
  printf("\teven nos.: %i\n", info.even_sum);
  printf("\todd nos.: %i\n", info.odd_sum);

  free(numbers);
  return 0;
}
