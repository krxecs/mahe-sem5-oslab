#include "vec.h"

void ay_vecbase_expand(ay_vecbase *base, size_t item_size) {
  base->capacity *= 2;
  void *new_data = realloc(base + 1, base->capacity * item_size);
  if (!new_data) {
    perror("realloc");
    exit(EXIT_FAILURE);
  }
  *(void **)(base + 1) = new_data;
}
