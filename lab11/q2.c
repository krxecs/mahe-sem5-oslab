#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int NUM_PROCESSES;
int NUM_RESOURCES;

int *available;
int **maximum;
int **allocation;
int **need;

pthread_mutex_t bank_mutex;

bool isSafe() {
  int *work = malloc(NUM_RESOURCES * sizeof work[0]);
  for (int i = 0; i < NUM_RESOURCES; i++)
    work[i] = available[i];

  bool *finish = calloc(NUM_PROCESSES, sizeof finish[0]);
  int count = 0;

  while (count < NUM_PROCESSES) {
    bool found = false;
    for (int p = 0; p < NUM_PROCESSES; p++) {
      if (!finish[p]) {
        bool possible = true;
        for (int j = 0; j < NUM_RESOURCES; j++) {
          if (need[p][j] > work[j]) {
            possible = false;
            break;
          }
        }
        if (possible) {
          for (int k = 0; k < NUM_RESOURCES; k++)
            work[k] += allocation[p][k];
          finish[p] = true;
          count++;
          found = true;
        }
      }
    }
    if (!found) {
      free(work);
      free(finish);
      return false;
    }
  }
  free(work);
  free(finish);
  return true;
}

bool request_resources(int thread_id, int request[]) {
  pthread_mutex_lock(&bank_mutex);
  printf("Thread %d requesting resources...\n", thread_id);

  bool granted = true;
  for (int i = 0; i < NUM_RESOURCES; i++) {
    if (request[i] > need[thread_id][i] || request[i] > available[i]) {
      granted = false;
      break;
    }
  }

  if (granted) {
    for (int i = 0; i < NUM_RESOURCES; i++) {
      available[i] -= request[i];
      allocation[thread_id][i] += request[i];
      need[thread_id][i] -= request[i];
    }

    if (!isSafe()) {
      printf("Request by Thread %d leads to unsafe state. Rolling back.\n",
             thread_id);
      for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] += request[i];
        allocation[thread_id][i] -= request[i];
        need[thread_id][i] += request[i];
      }
      granted = false;
    } else
      printf("Request by Thread %d granted.\n", thread_id);
  } else
    printf("Request by Thread %d denied (exceeds need or not available).\n",
           thread_id);

  pthread_mutex_unlock(&bank_mutex);
  return granted;
}

void release_resources(int thread_id, int release[]) {
  pthread_mutex_lock(&bank_mutex);
  printf("Thread %d releasing resources...\n", thread_id);
  for (int i = 0; i < NUM_RESOURCES; i++) {
    allocation[thread_id][i] -= release[i];
    available[i] += release[i];
    need[thread_id][i] += release[i];
  }
  pthread_mutex_unlock(&bank_mutex);
}

void *process_thread(void *arg) {
  int thread_id = *(int *)arg;

  for (int i = 0; i < 3; i++) {
    sleep(rand() % 3);
    int *request = calloc(NUM_RESOURCES, sizeof request[0]);
    for (int j = 0; j < NUM_RESOURCES; j++)
      if (need[thread_id][j] > 0)
        request[j] = rand() % (need[thread_id][j] + 1);

    if (request_resources(thread_id, request)) {
      sleep(rand() % 3);
      release_resources(thread_id, request);
    }
    free(request);
  }

  free(arg);
  return NULL;
}

int main(void) {
  srand(time(NULL));
  pthread_mutex_init(&bank_mutex, NULL);

  printf("Enter number of processes: ");
  scanf("%d", &NUM_PROCESSES);
  printf("Enter number of resources: ");
  scanf("%d", &NUM_RESOURCES);

  available = malloc(NUM_RESOURCES * sizeof available[0]);
  maximum = malloc(NUM_PROCESSES * sizeof maximum[0]);
  allocation = malloc(NUM_PROCESSES * sizeof allocation[0]);
  need = malloc(NUM_PROCESSES * sizeof need[0]);
  for (int i = 0; i < NUM_PROCESSES; i++) {
    maximum[i] = malloc(NUM_RESOURCES * sizeof maximum[0][0]);
    allocation[i] = calloc(NUM_RESOURCES, sizeof allocation[0][0]);
    need[i] = malloc(NUM_RESOURCES * sizeof need[0][0]);
  }

  printf("Enter available instances for each resource: ");
  for (int i = 0; i < NUM_RESOURCES; i++)
    scanf("%d", &available[i]);

  puts("Enter maximum need for each process:");
  for (int i = 0; i < NUM_PROCESSES; i++) {
    printf("P%d: ", i);
    for (int j = 0; j < NUM_RESOURCES; j++) {
      scanf("%d", &maximum[i][j]);
      need[i][j] = maximum[i][j];
    }
  }

  pthread_t *threads = malloc(NUM_PROCESSES * sizeof threads[0]);

  for (int i = 0; i < NUM_PROCESSES; i++) {
    int *thread_id = malloc(sizeof *thread_id);
    *thread_id = i;
    if (pthread_create(&threads[i], NULL, process_thread, thread_id)) {
      perror("Failed to create thread");
      return 1;
    }
  }

  for (int i = 0; i < NUM_PROCESSES; i++)
    pthread_join(threads[i], NULL);

  pthread_mutex_destroy(&bank_mutex);
  puts("\nAll processes finished.");

  free(available);
  for (int i = 0; i < NUM_PROCESSES; i++) {
    free(maximum[i]);
    free(allocation[i]);
    free(need[i]);
  }
  free(maximum);
  free(allocation);
  free(need);
  free(threads);

  return 0;
}
