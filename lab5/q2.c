#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  puts("In main program");
  pid_t cpid = fork();
  if (cpid < 0) {
    perror("Error forking");
    return 1;
  } else if (cpid == 0) {
    if (execl("./lab5-q1", "./lab5-q1", NULL) < 0) {
      perror("Error execl");
      return 1;
    }
  } else {
    wait(NULL);
    puts("Child process exited.");
  }
  return 0;
}
