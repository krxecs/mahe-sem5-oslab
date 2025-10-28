#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 100
#define SPARE_BLOCKS 10
#define MAX_FILENAME 20

int *disk;
int *spare_pool;
int spare_pool_idx = 0;

typedef struct {
  int id;
  char name[MAX_FILENAME];
  int start_block;
  int length;
} FileInfo;

FileInfo file_table[MAX_BLOCKS];
int file_count = 0;

void initialize_disk() {
  disk = malloc(MAX_BLOCKS * sizeof disk[0]);
  spare_pool = malloc(SPARE_BLOCKS * sizeof spare_pool[0]);

  for (int i = 0; i < MAX_BLOCKS; i++)
    disk[i] = 0;
  for (int i = 0; i < SPARE_BLOCKS; i++)
    spare_pool[i] = MAX_BLOCKS + i;
}

void mark_bad_blocks() {
  int num_bad, block_num;
  printf("Enter number of bad blocks: ");
  scanf("%d", &num_bad);
  puts("Enter the bad block numbers:");
  for (int i = 0; i < num_bad; i++) {
    scanf("%d", &block_num);
    if (block_num >= 0 && block_num < MAX_BLOCKS)
      disk[block_num] = -1;
    else
      printf("Invalid block number: %d\n", block_num);
  }
}

void allocate_file() {
  char name[MAX_FILENAME];
  int length;
  printf("Enter file name: ");
  scanf("%s", name);
  printf("Enter file length (number of blocks): ");
  scanf("%d", &length);

  int start_block = -1;
  bool found_space = false;

  for (int i = 0; i <= MAX_BLOCKS - length; i++) {
    bool is_contiguous = true;
    for (int j = 0; j < length; j++) {
      if (disk[i + j] != 0) {
        is_contiguous = false;
        break;
      }
    }
    if (is_contiguous) {
      start_block = i;
      found_space = true;
      break;
    }
  }

  if (!found_space) {
    printf("Error: Not enough contiguous space for file '%s'.\n", name);
    return;
  }

  for (int i = 0; i < length; i++) {
    if (disk[start_block + i] == -1) {
      if (spare_pool_idx < SPARE_BLOCKS) {
        printf("Bad block %d encountered. Remapping to spare block %d.\n",
               start_block + i, spare_pool[spare_pool_idx]);
        spare_pool_idx++;
      } else {
        puts("Error: Bad block encountered but no spare blocks available. "
             "Allocation failed.");
        return;
      }
    }
  }

  int file_id = file_count + 1;
  for (int i = 0; i < length; i++)
    disk[start_block + i] = file_id;

  file_table[file_count].id = file_id;
  strncpy(file_table[file_count].name, name, MAX_FILENAME);
  file_table[file_count].start_block = start_block;
  file_table[file_count].length = length;
  file_count++;

  printf("File '%s' allocated successfully at block %d.\n", name, start_block);
}

void print_disk_map() {
  puts("\n--- Disk Block Map ---");
  for (int i = 0; i < MAX_BLOCKS; i++) {
    if (i % 10 == 0 && i != 0)
      puts("");

    if (disk[i] == 0)
      printf("[ ] ");
    else if (disk[i] == -1)
      printf("[B] ");
    else
      printf("[%d] ", disk[i]);
  }
  puts("\n\n--- File Table ---");
  puts("ID | Name       | Start | Length");
  puts("--------------------------------");
  for (int i = 0; i < file_count; i++)
    printf("%-2d | %-10s | %-5d | %d\n", file_table[i].id, file_table[i].name,
           file_table[i].start_block, file_table[i].length);
  puts("");
}

int main(void) {
  initialize_disk();
  mark_bad_blocks();

  int choice;
  while (1) {
    puts("\n1. Allocate File");
    puts("2. Print Disk Map");
    puts("3. Exit");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      allocate_file();
      break;
    case 2:
      print_disk_map();
      break;
    case 3:
      free(disk);
      free(spare_pool);
      return 0;
    default:
      puts("Invalid choice. Please try again.");
    }
  }

  return 0;
}
