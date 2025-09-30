#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/uio.h>

void perrf(bool b, const char *s) {
  if (b) {
    perror(s);
    exit(EXIT_FAILURE);
  }
}

void perrf_free(bool b, void *p, const char *s) {
  if (b) {
    perror(s);
    free(p);
    exit(EXIT_FAILURE);
  }
}

void fd_to_file(int n, FILE *dst[n], const char *m[n], int src[n]) {
  for (int i = 0; i < n; ++i) {
    dst[i] = fdopen(src[i], m[i]);
    perrf(dst[i] == NULL, "fd_to_file");
  }
}

void fdpipe_to_file(FILE *dst[2], int src[2]) {
  const char *m[2] = {"r", "w"};
  fd_to_file(2, dst, m, src);
}

void u32_to_be32(unsigned char b[4], uint32_t x) {
  b[0] = (x >> 24) & 0xff;
  b[1] = (x >> 16) & 0xff;
  b[2] = (x >>  8) & 0xff;
  b[3] = (x      ) & 0xff;
}

uint32_t be32_to_u32(unsigned char b[4]) {
  return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

ssize_t getline_from_fd(char **lineptr, size_t *n, int fd) {
  static const size_t INITIAL_BUF_SIZE = 128;
  size_t i = 0;
  char c;

  if (*lineptr == NULL || *n == 0) {
    *lineptr = malloc(INITIAL_BUF_SIZE);
    if (!*lineptr)
      return -1;

    *n = INITIAL_BUF_SIZE;
  }

  ssize_t bytes_read;
  while ((bytes_read = read(fd, &c, 1)) > 0) {
    if (i >= *n - 1) {
      size_t new_size = *n * 2;
      char *new_buf = realloc(*lineptr, new_size);
      if (!new_buf)
        return -1;

      *lineptr = new_buf;
      *n = new_size;
    }

    (*lineptr)[i++] = c;

    if (c == '\n')
      break;
  }

  if (bytes_read <= 0 && i == 0)
    return -1;

  (*lineptr)[i] = '\0';
  return i;
}

ssize_t read_exact(int fd, void *buf, size_t count) {
  size_t total_read = 0;
  char *cbuf = (char *)buf;

  while (total_read < count) {
    ssize_t n = read(fd, cbuf + total_read, count - total_read);
    if (n <= 0) {
      if (n == 0) {
        // EOF
        return total_read;
      }
      // Error
      return n;
    }
    total_read += n;
  }
  return total_read;
}
