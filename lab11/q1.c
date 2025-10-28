#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void printMatrix(int **matrix, int rows, int cols, const char *name) {
  printf("\n%s Matrix:\n   ", name);
  for (int j = 0; j < cols; j++)
    printf(" R%d", j);
  puts("");
  for (int i = 0; i < rows; i++) {
    printf("P%d ", i);
    for (int j = 0; j < cols; j++)
      printf("%2d ", matrix[i][j]);
    puts("");
  }
}

void printVector(int *vector, int size, const char *name) {
  printf("\n%s Vector:\n", name);
  for (int i = 0; i < size; i++)
    printf("R%d ", i);
  puts("");
  for (int i = 0; i < size; i++)
    printf("%d  ", vector[i]);
  puts("");
}

bool isSafe(int **allocation, int **need, int *available, int num_processes,
            int num_resources) {
  int *work = malloc(num_resources * sizeof work[0]);
  for (int i = 0; i < num_resources; i++)
    work[i] = available[i];

  bool *finish = calloc(num_processes, sizeof finish[0]);
  int *safeSequence = malloc(num_processes * sizeof safeSequence[0]);
  int count = 0;

  while (count < num_processes) {
    bool found = false;
    for (int p = 0; p < num_processes; p++) {
      if (!finish[p]) {
        bool can_allocate = true;
        for (int j = 0; j < num_resources; j++) {
          if (need[p][j] > work[j]) {
            can_allocate = false;
            break;
          }
        }

        if (can_allocate) {
          for (int k = 0; k < num_resources; k++)
            work[k] += allocation[p][k];
          safeSequence[count++] = p;
          finish[p] = found = true;
        }
      }
    }
    if (!found) {
      puts("\nSystem is not in a safe state.");
      free(work);
      free(finish);
      free(safeSequence);
      return false;
    }
  }

  printf("\nSystem is in a safe state.\nSafe sequence is: < ");
  for (int i = 0; i < num_processes; i++)
    printf("P%d ", safeSequence[i]);
  puts(">");

  free(work);
  free(finish);
  free(safeSequence);
  return true;
}

void resourceRequest(int process_id, int *request, int **allocation, int **need,
                     int *available, int num_processes, int num_resources) {
  puts("\n------------------------------------------------------");
  printf("Processing request from P%d...\n", process_id);

  for (int i = 0; i < num_resources; i++) {
    if (request[i] > need[process_id][i]) {
      puts("Error: Process has exceeded its maximum claim. Request denied.");
      return;
    }
  }

  for (int i = 0; i < num_resources; i++) {
    if (request[i] > available[i]) {
      puts("Process must wait. Resources are not available. Request denied.");
      return;
    }
  }

  int *temp_available = malloc(num_resources * sizeof temp_available[0]);
  int **temp_allocation = malloc(num_processes * sizeof temp_allocation[0]);
  int **temp_need = malloc(num_processes * sizeof temp_need[0]);
  for (int i = 0; i < num_processes; i++) {
    temp_allocation[i] = malloc(num_resources * sizeof temp_allocation[0][0]);
    temp_need[i] = malloc(num_resources * sizeof temp_need[0][0]);
  }

  for (int i = 0; i < num_resources; i++)
    temp_available[i] = available[i] - request[i];
  for (int i = 0; i < num_processes; i++)
    for (int j = 0; j < num_resources; j++) {
      temp_allocation[i][j] = allocation[i][j];
      temp_need[i][j] = need[i][j];
    }
  for (int i = 0; i < num_resources; i++) {
    temp_allocation[process_id][i] += request[i];
    temp_need[process_id][i] -= request[i];
  }

  if (isSafe(temp_allocation, temp_need, temp_available, num_processes,
             num_resources)) {
    puts("Request can be granted immediately.");
    for (int i = 0; i < num_resources; i++) {
      available[i] = temp_available[i];
      allocation[process_id][i] = temp_allocation[process_id][i];
      need[process_id][i] = temp_need[process_id][i];
    }

    printMatrix(allocation, num_processes, num_resources, "Updated Allocation");
    printMatrix(need, num_processes, num_resources, "Updated Need");
    printVector(available, num_resources, "Updated Available");

  } else
    puts("Request cannot be granted as it would lead to an unsafe state.");
  puts("------------------------------------------------------");

  free(temp_available);
  for (int i = 0; i < num_processes; i++) {
    free(temp_allocation[i]);
    free(temp_need[i]);
  }
  free(temp_allocation);
  free(temp_need);
}

int main(void) {
  int num_processes, num_resources;

  printf("Enter the number of processes: ");
  scanf("%d", &num_processes);

  printf("Enter the number of resources: ");
  scanf("%d", &num_resources);

  int **allocation = malloc(num_processes * sizeof allocation[0]);
  int **max = malloc(num_processes * sizeof max[0]);
  int **need = malloc(num_processes * sizeof need[0]);
  for (int i = 0; i < num_processes; i++) {
    allocation[i] = malloc(num_resources * sizeof allocation[0][0]);
    max[i] = malloc(num_resources * sizeof max[0][0]);
    need[i] = malloc(num_resources * sizeof need[0][0]);
  }
  int *available = malloc(num_resources * sizeof available[0]);

  puts("\nEnter the Allocation Matrix:");
  for (int i = 0; i < num_processes; i++)
    for (int j = 0; j < num_resources; j++)
      scanf("%d", &allocation[i][j]);

  puts("\nEnter the Max Matrix:");
  for (int i = 0; i < num_processes; i++)
    for (int j = 0; j < num_resources; j++)
      scanf("%d", &max[i][j]);

  puts("\nEnter the Available Resources Vector:");
  for (int i = 0; i < num_resources; i++)
    scanf("%d", &available[i]);

  for (int i = 0; i < num_processes; i++)
    for (int j = 0; j < num_resources; j++)
      need[i][j] = max[i][j] - allocation[i][j];

  printMatrix(need, num_processes, num_resources, "Calculated Need");

  isSafe(allocation, need, available, num_processes, num_resources);

  char choice = 'y';
  while (choice == 'y') {
    int process_id;
    int *request = malloc(num_resources * sizeof request[0]);

    printf("\nDo you want to make a resource request? (y/n): ");
    scanf(" %c", &choice);
    choice = tolower(choice);

    if (choice == 'y') {
      printf("Enter process ID making the request (0 to %d): ",
             num_processes - 1);
      scanf("%d", &process_id);

      if (process_id >= num_processes || process_id < 0) {
        puts("Invalid process ID.");
        free(request);
        continue;
      }

      printf("Enter the resource request vector: ");
      for (int i = 0; i < num_resources; i++)
        scanf("%d", &request[i]);

      resourceRequest(process_id, request, allocation, need, available,
                      num_processes, num_resources);
      free(request);
    }
  }

  for (int i = 0; i < num_processes; i++) {
    free(allocation[i]);
    free(max[i]);
    free(need[i]);
  }
  free(allocation);
  free(max);
  free(need);
  free(available);

  return 0;
}
