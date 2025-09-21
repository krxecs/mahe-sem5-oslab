#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

void perrf(bool b, const char *s);
void fd_to_file(int n, FILE *dst[n], const char *m[n], int src[n]);
void fdpipe_to_file(FILE *dst[2], int src[2]);
void u32_to_be32(unsigned char b[4], uint32_t x);
uint32_t be32_to_u32(unsigned char b[4]);
ssize_t getline_from_fd(char **lineptr, size_t *n, int fd);
ssize_t read_exact(int fd, void *buf, size_t count);

#endif /* HELPER_H_INCLUDED */
