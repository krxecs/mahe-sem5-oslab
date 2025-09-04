#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
  pid_t child_pid;

  child_pid = fork();

  if (child_pid > 0) {
    printf("Parent process (PID %d) is sleeping.\n", getpid());
    sleep(30);
    printf("Parent process (PID %d) is now terminating.\n", getpid());
  }
  else if (child_pid == 0) {
    printf("Child process (PID %d) is exiting now.\n", getpid());
    exit(0);
  }
  else {
    perror("fork() failed");
    return 1;
  }

  return 0;
}

