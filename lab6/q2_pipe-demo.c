#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "helper.h"

static const char *FIFO_NAME = "q2.fifo";

void *write_thrd(void *arg) {
  int fd = open(FIFO_NAME, O_WRONLY);
  perrf(fd < 0, "write_thrd open");

  int r = write(fd, "Hello", 5);
  perrf(r < 5, "write_thrd write");
  printf("Write thread: %s\n", "Hello");

  close(fd);
  return NULL;
}

void *read_thrd(void *arg) {
  int fd = open(FIFO_NAME, O_RDONLY);
  perrf(fd < 0, "read_thrd open");

  char buf[6];
  int r = read(fd, buf, 5);
  perrf(r < 5, "read_thrd read");
  buf[5] = 0;
  printf("Read thread: %s\n", buf);

  close(fd);
  return NULL;
}

int main(void) {
  pthread_t thrds[2];

  perrf(mkfifo(FIFO_NAME, 0600) != 0, "fifo");
  pthread_create(&thrds[0], NULL, write_thrd, NULL);
  pthread_create(&thrds[1], NULL, read_thrd,  NULL);

  pthread_join(thrds[0], NULL);
  pthread_join(thrds[1], NULL);

  unlink(FIFO_NAME);
  return 0;
}
