#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define AY_VEC_INITIAL_CAPACITY 8

typedef struct ay_vecbase {
  size_t size;
  size_t capacity;
} ay_vecbase;

#define ay_vec(T) \
  struct { \
    ay_vecbase base; \
    T *data; \
  }

#define ay_vec_init(vecaddr) \
  do { \
    (vecaddr)->base = (ay_vecbase){0, AY_VEC_INITIAL_CAPACITY}; \
    (vecaddr)->data = malloc((vecaddr)->base.capacity * sizeof(*(vecaddr)->data)); \
    if (!(vecaddr)->data) { \
      perror("malloc"); \
      exit(1); \
    } \
  } while (0)

#define ay_vec_push_back(vecaddr, value) \
  do { \
    if ((vecaddr)->base.size == (vecaddr)->base.capacity) { \
      ay_vecbase_expand(&(vecaddr)->base, sizeof(*(vecaddr)->data)); \
    } \
    (vecaddr)->data[(vecaddr)->base.size++] = (value); \
  } while (0)

#define ay_vec_delete(vecaddr, idx) \
  do { \
    if ((idx) < (vecaddr)->base.size) { \
      memmove(&(vecaddr)->data[(idx)], &(vecaddr)->data[(idx) + 1], ((vecaddr)->base.size - (idx) - 1) * sizeof(*(vecaddr)->data)); \
      (vecaddr)->base.size--; \
    } \
  } while (0)

#define ay_vec_free(vecaddr) \
  do { \
    free((vecaddr)->data); \
    (vecaddr)->data = NULL; \
    (vecaddr)->base = (ay_vecbase){0, 0}; \
  } while (0)

void ay_vecbase_expand(ay_vecbase *base, size_t item_size);

#endif
