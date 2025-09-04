#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 2048

void report_error(const char *message) {
  write(STDERR_FILENO, message, strlen(message));
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    report_error("Usage: ./simple_grep <file> <word>\n");
    exit(EXIT_FAILURE);
  }

  const char *filename = argv[1];
  const char *search_word = argv[2];

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    report_error("Error: Cannot open the specified file.\n");
    exit(EXIT_FAILURE);
  }

  char read_char;
  char line_buffer[MAX_LINE_LENGTH];
  int line_pos = 0;
  ssize_t bytes_read;

  while ((bytes_read = read(fd, &read_char, 1)) > 0) {
    if (read_char != '\n') {
      if (line_pos < MAX_LINE_LENGTH - 1)
        line_buffer[line_pos++] = read_char;
    } else {
      line_buffer[line_pos] = '\0';

      if (strstr(line_buffer, search_word) != NULL) {
        write(STDOUT_FILENO, line_buffer, line_pos);
        write(STDOUT_FILENO, "\n", 1);
      }

      line_pos = 0;
    }
  }
  
  if (line_pos > 0) {
    line_buffer[line_pos] = '\0';
    if (strstr(line_buffer, search_word) != NULL) {
      write(STDOUT_FILENO, line_buffer, line_pos);
      write(STDOUT_FILENO, "\n", 1);
    }
  }

  if (bytes_read == -1)
    report_error("Error: An error occurred while reading the file.\n");

  close(fd);

  return 0;
}

