#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

void perrf(bool b, const char *s) {
  if (b) {
    perror(s);
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
