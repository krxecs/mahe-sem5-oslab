#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

void fcfs(int requests[], int n, int head) {
  puts("\n--- FCFS Scheduling ---");
  int total_seek = 0;
  int current_head = head;

  printf("Seek Sequence: %d", current_head);

  for (int i = 0; i < n; i++) {
    int diff = abs(requests[i] - current_head);
    total_seek += diff;
    current_head = requests[i];
    printf(" -> %d", current_head);
  }

  printf("\nTotal Head Movement: %d cylinders\n", total_seek);
}

void sstf(int requests[], int n, int head) {
  puts("\n--- SSTF Scheduling ---");
  int total_seek = 0;
  int current_head = head;

  int *req_copy = malloc(n * sizeof req_copy[0]);
  for (int i = 0; i < n; i++)
    req_copy[i] = requests[i];

  printf("Seek Sequence: %d", current_head);

  for (int i = 0; i < n; i++) {
    int min_diff = INT_MAX;
    int next_req_index = -1;

    for (int j = 0; j < n; j++) {
      if (req_copy[j] != -1) {
        int diff = abs(req_copy[j] - current_head);
        if (diff < min_diff) {
          min_diff = diff;
          next_req_index = j;
        }
      }
    }

    if (next_req_index != -1) {
      total_seek += min_diff;
      current_head = req_copy[next_req_index];
      req_copy[next_req_index] = -1;
      printf(" -> %d", current_head);
    }
  }
  free(req_copy);
  printf("\nTotal Head Movement: %d cylinders\n", total_seek);
}

int main(void) {
  int n, head;
  int requests[MAX_REQUESTS];

  printf("Enter the number of disk requests: ");
  scanf("%d", &n);
  if (n > MAX_REQUESTS || n <= 0) {
    puts("Invalid number of requests.");
    return 1;
  }

  puts("Enter the request queue (cylinder numbers):");
  for (int i = 0; i < n; i++)
    scanf("%d", &requests[i]);

  printf("Enter the initial head position: ");
  scanf("%d", &head);

  fcfs(requests, n, head);
  sstf(requests, n, head);

  return 0;
}
