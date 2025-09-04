#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
  printf("Starting child process... ");
  fflush(stdout);
  pid_t cpid = fork();
  if (cpid < 0) {
    perror("Error forking");
  } else if (cpid == 0) {
    puts("Started\nIn child process.");
  } else {
    int status;
    wait(&status);
    puts("Child stopped.");
  }
  return 0;
}
