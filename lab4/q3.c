#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

void report_error(const char *s) {
  write(STDERR_FILENO, s, strlen(s));
  write(STDERR_FILENO, "\n", 1);
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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    report_error("Usage: ./lab4-q3 <file>");
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    report_error("Error: Cannot open file.\n");
    return EXIT_FAILURE;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read_len;
  int i = 0;

  fcntl (STDIN_FILENO, F_SETFL, O_NONBLOCK);
  while ((read_len = getline_from_fd(&line, &len, fd)) != -1) {
    write(STDOUT_FILENO, line, read_len);
    if (i % 20 == 0 && i != 0) {
      write(STDOUT_FILENO, "Enter key: ", 11);
      int b = 1;
      for (; b;) {
        char c[2];
        if (read(STDIN_FILENO, &c, 2) > 0) b = 0;
      }
      write(STDOUT_FILENO, "\n", 1);
    }
    i++;
  }

  free(line);
  close(fd);
  return 0;
}
