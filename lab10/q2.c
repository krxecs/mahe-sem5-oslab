#include <stdio.h>
#include <stdlib.h>

#define NUM_QUEUES 3
#define TIME_SLICE_Q1 4
#define TIME_SLICE_Q2 8

typedef struct {
  int pid;
  int burst_time;
  int remaining_time;
  int queue_level;
} PCB;

typedef struct Node {
  PCB process;
  struct Node *next;
} Node;

typedef struct {
  Node *front, *rear;
} Queue;

PCB create_process(int pid, int burst) {
  return (PCB){.pid = pid,
               .burst_time = burst,
               .remaining_time = burst,
               .queue_level = 0};
}

Queue *create_queue() {
  Queue *q = (Queue *)malloc(sizeof(Queue));
  if (!q) {
    perror("Failed to create queue");
    exit(EXIT_FAILURE);
  }
  q->front = q->rear = NULL;
  return q;
}

void enqueue(Queue *q, PCB process) {
  Node *temp = (Node *)malloc(sizeof(Node));
  if (!temp) {
    perror("Failed to create queue node");
    exit(EXIT_FAILURE);
  }
  *temp = (Node){.process = process, .next = NULL};

  if (!q->rear) {
    q->front = q->rear = temp;
    return;
  }
  q->rear->next = temp;
  q->rear = temp;
}

PCB dequeue(Queue *q) {
  if (!q->front) {
    return (PCB){-1, -1, -1, -1};
  }
  Node *temp = q->front;
  PCB process = temp->process;
  q->front = q->front->next;
  if (!q->front) {
    q->rear = NULL;
  }
  free(temp);
  return process;
}

void simulate_scheduler(Queue *queues[], int num_processes) {
  int time = 0;
  int processes_finished = 0;

  while (processes_finished < num_processes) {
    int moved = 0;
    if (queues[0]->front) {
      PCB p = dequeue(queues[0]);
      printf("Time %d: Process %d running from Q1 (RR, slice=4)\n", time,
             p.pid);
      int run_time =
          (p.remaining_time < TIME_SLICE_Q1) ? p.remaining_time : TIME_SLICE_Q1;
      p.remaining_time -= run_time;
      time += run_time;
      if (p.remaining_time <= 0) {
        printf("Time %d: Process %d finished.\n", time, p.pid);
        processes_finished++;
      } else {
        printf("Time %d: Process %d moved to Q2.\n", time, p.pid);
        p.queue_level = 1;
        enqueue(queues[1], p);
      }
      moved = 1;
    } else if (queues[1]->front) {
      PCB p = dequeue(queues[1]);
      printf("Time %d: Process %d running from Q2 (RR, slice=8)\n", time,
             p.pid);
      int run_time =
          (p.remaining_time < TIME_SLICE_Q2) ? p.remaining_time : TIME_SLICE_Q2;
      p.remaining_time -= run_time;
      time += run_time;
      if (p.remaining_time <= 0) {
        printf("Time %d: Process %d finished.\n", time, p.pid);
        processes_finished++;
      } else {
        printf("Time %d: Process %d moved to Q3.\n", time, p.pid);
        p.queue_level = 2;
        enqueue(queues[2], p);
      }
      moved = 1;
    } else if (queues[2]->front) {
      PCB p = dequeue(queues[2]);
      printf("Time %d: Process %d running from Q3 (FCFS)\n", time, p.pid);
      time += p.remaining_time;
      p.remaining_time = 0;
      printf("Time %d: Process %d finished.\n", time, p.pid);
      processes_finished++;
      moved = 1;
    }

    if (!moved) {
      time++;
    }
  }
}

int main() {
  Queue *queues[NUM_QUEUES];
  for (int i = 0; i < NUM_QUEUES; i++) {
    queues[i] = create_queue();
  }

  PCB p1 = create_process(1, 20);
  PCB p2 = create_process(2, 5);
  PCB p3 = create_process(3, 12);
  PCB p4 = create_process(4, 3);

  enqueue(queues[0], p1);
  enqueue(queues[0], p2);
  enqueue(queues[0], p3);
  enqueue(queues[0], p4);

  simulate_scheduler(queues, 4);

  for (int i = 0; i < NUM_QUEUES; ++i) {
    free(queues[i]);
  }

  return 0;
}
