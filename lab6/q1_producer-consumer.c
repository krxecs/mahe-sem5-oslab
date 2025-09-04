#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "helper.h"

int main(void) {
  int data_pipe[2], ack_pipe[2];
  perrf(pipe(data_pipe) < 0, "pipe");
  perrf(pipe(ack_pipe)  < 0, "pipe");

  pid_t cpid = fork();
  perrf(cpid < 0, "fork");

  if (cpid == 0) {
    close(data_pipe[1]);
    close(ack_pipe[0]);

    for (int i = 0; i < 4; ++i) {
      unsigned char buf[4];
      perrf(read(data_pipe[0], buf, sizeof buf) < sizeof buf, "consumer read error");
      uint32_t x = be32_to_u32(buf);
      printf("Consumer: %i\n", x);

      unsigned char ack = 1;
      perrf(write(ack_pipe[1], &ack, 1) < 1, "consumer write error");
    }

    close(data_pipe[0]);
    close(ack_pipe[1]);
  } else {
    close(data_pipe[0]);
    close(ack_pipe[1]);

    uint32_t a[4] = {1, 2, 3, 4};
    for (int i = 0; i < 4; ++i) {
      printf("Producer: %i\n", a[i]);
      unsigned char buf[4];
      u32_to_be32(buf, a[i]);
      perrf(write(data_pipe[1], buf, sizeof buf) < sizeof buf, "producer write error");

      for (unsigned char ack = 0; !ack; perrf(read(ack_pipe[0], &ack, 1) < 1, "producer read error"));
    }

    wait(NULL);
    close(data_pipe[1]);
    close(ack_pipe[0]);
  }

  return 0;
}
