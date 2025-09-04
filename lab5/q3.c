#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t child_pid;

  child_pid = fork();

  if (child_pid > 0) {
    wait(NULL);
    printf("\n--- Executing in the PARENT process ---\n");
    printf("Parent's Process ID (PID): %d\n", getpid());
    printf("Parent's Parent Process ID (PPID): %d\n", getppid());
    printf("Child's Process ID (from parent's perspective): %d\n", child_pid);
    printf("---------------------------------------\n");
  }
  else if (child_pid == 0) {
    printf("\n--- Executing in the CHILD process ---\n");
    printf("Child's Process ID (PID): %d\n", getpid());
    printf("Child's Parent Process ID (PPID): %d\n", getppid());
    printf("Child's Child Process ID: N/A\n");
    printf("--------------------------------------\n");
  }
  else {
    perror("fork() failed");
    return 1;
  }

  return 0;
}

