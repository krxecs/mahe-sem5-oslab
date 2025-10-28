#include <stdio.h>
#include <stdlib.h>

#define MAX_FRAMES 3
#define MAX_PAGES 20

typedef struct {
  int page_num;
  int frame_num;
  int valid;
  int reference_bit;
} Page;

int frames[MAX_FRAMES];
Page page_table[MAX_PAGES];

int find_page(int page_num) {
  for (int i = 0; i < MAX_PAGES; i++) {
    if (page_table[i].page_num == page_num) {
      return i;
    }
  }
  return -1;
}

void simulate_second_chance(int ref_string[], int n) {
  int frame_pointer = 0;
  int page_faults = 0;

  for (int i = 0; i < MAX_FRAMES; i++) {
    frames[i] = -1;
  }

  printf("Reference String: ");
  for (int i = 0; i < n; ++i)
    printf("%d ", ref_string[i]);
  printf("\n\n");

  for (int i = 0; i < n; i++) {
    int current_page = ref_string[i];
    int page_index = find_page(current_page);

    printf("Accessing page %d: ", current_page);

    if (page_table[page_index].valid) {
      printf("Hit (Frame %d)\n", page_table[page_index].frame_num);
      page_table[page_index].reference_bit = 1;
    } else {
      page_faults++;
      printf("Fault, ");

      while (1) {
        int victim_frame_page_index = -1;
        for (int j = 0; j < MAX_PAGES; ++j) {
          if (page_table[j].valid && page_table[j].frame_num == frame_pointer) {
            victim_frame_page_index = j;
            break;
          }
        }

        if (victim_frame_page_index == -1) {
          page_table[page_index].valid = 1;
          page_table[page_index].frame_num = frame_pointer;
          page_table[page_index].reference_bit = 0;
          frames[frame_pointer] = current_page;
          printf("placed in empty frame %d.\n", frame_pointer);
          frame_pointer = (frame_pointer + 1) % MAX_FRAMES;
          break;
        } else if (page_table[victim_frame_page_index].reference_bit == 0) {
          printf("replacing page %d in frame %d.\n",
                 page_table[victim_frame_page_index].page_num, frame_pointer);
          page_table[victim_frame_page_index].valid = 0;
          page_table[page_index].valid = 1;
          page_table[page_index].frame_num = frame_pointer;
          page_table[page_index].reference_bit = 0;
          frames[frame_pointer] = current_page;

          frame_pointer = (frame_pointer + 1) % MAX_FRAMES;
          break;
        } else {
          page_table[victim_frame_page_index].reference_bit = 0;
          frame_pointer = (frame_pointer + 1) % MAX_FRAMES;
        }
      }
    }

    printf("Frames: ");
    for (int k = 0; k < MAX_FRAMES; ++k) {
      if (frames[k] == -1)
        printf("[ ] ");
      else
        printf("[%d] ", frames[k]);
    }
    printf("\n\n");
  }

  printf("Total Page Faults: %d\n", page_faults);
  printf("Hit Ratio: %.2f%%\n", ((double)(n - page_faults) / n) * 100);
}

int main() {
  int reference_string[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2, 3, 4};
  int n = sizeof(reference_string) / sizeof(reference_string[0]);

  for (int i = 0; i < MAX_PAGES; i++) {
    page_table[i] =
        (Page){.page_num = i, .frame_num = -1, .valid = 0, .reference_bit = 0};
  }

  simulate_second_chance(reference_string, n);

  return 0;
}
