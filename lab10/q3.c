#include <stdio.h>
#include <stdlib.h>

#define NUM_SEGMENTS 5

typedef struct {
  int limit;
  int base;
} Segment;

Segment segment_table[NUM_SEGMENTS];

void initialize_segment_table() {
  segment_table[0].base = 1400;
  segment_table[0].limit = 1000;
  segment_table[1].base = 6300;
  segment_table[1].limit = 400;
  segment_table[2].base = 4300;
  segment_table[2].limit = 400;
  segment_table[3].base = 3200;
  segment_table[3].limit = 1100;
  segment_table[4].base = 4700;
  segment_table[4].limit = 1000;
}

void translate_address(int segment_num, int offset) {
  printf("Logical Address: Segment %d, Offset %d\n", segment_num, offset);

  if (segment_num < 0 || segment_num >= NUM_SEGMENTS) {
    printf("Error: Invalid segment number.\n\n");
    return;
  }

  if (offset < 0 || offset >= segment_table[segment_num].limit) {
    printf("Error: Offset %d is out of bounds for segment %d (Limit: %d).\n\n",
           offset, segment_num, segment_table[segment_num].limit);
    return;
  }

  int physical_address = segment_table[segment_num].base + offset;
  printf("Physical Address: %d\n\n", physical_address);
}

int main() {
  initialize_segment_table();

  translate_address(2, 53);

  translate_address(3, 852);

  translate_address(0, 1222);

  translate_address(4, 1100);

  return 0;
}
