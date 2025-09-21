#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE 1

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <ay/aes.h>

#include "dbg.h"
#include "helper.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
    return 1;
  }

  const char *host = argv[1];
  const char *port = argv[2];

  struct addrinfo hints = {.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
  struct addrinfo *ai;
  int status = getaddrinfo(host, port, &hints, &ai);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  int sockfd = -1;
  for (struct addrinfo *p = ai; p != NULL; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1)
      continue;
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1)
      break;
    close(sockfd);
    sockfd = -1;
  }
  freeaddrinfo(ai);
  if (sockfd == -1) {
    fprintf(stderr, "Failed to connect\n");
    return 1;
  }

  struct pollfd pfds[2];
  pfds[0].fd = 0; // stdin
  pfds[0].events = POLLIN;
  pfds[1].fd = sockfd;
  pfds[1].events = POLLIN;

  for (;;) {
    int poll_count = poll(pfds, 2, -1);
    if (poll_count == -1) {
      perror("poll");
      break;
    }

    if (pfds[0].revents & POLLIN) {
      // Read from stdin
      size_t sz;
      char *linebuf = NULL;
      ssize_t n = getline_from_fd(&linebuf, &sz, 0);
      if (n <= 0)
        break;

      if (n >= UINT32_MAX) {
        fprintf(stderr, "Input too long\n");
        free(linebuf);
        continue;
      }
      if (linebuf[n - 1] == '\n')
        linebuf[--n] = '\0';

      uint8_t lenbuf[4];
      u32_to_be32(lenbuf, n);
      if (write(sockfd, lenbuf, sizeof lenbuf) == -1) {
        perror("write");
        free(linebuf);
        break;
      }

      static const uint8_t key[32] = {
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
          0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
          0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
      static const uint8_t iv[16] = {0};
      AesContext actx;
      aes_init(&actx, KEY_TYPE_AES256, key);

      uint8_t *enc_buf = malloc(n);
      aes_ctr_xcrypt(&actx, n, enc_buf, (const uint8_t *)linebuf, NULL, iv);

      if (write(sockfd, enc_buf, n) == -1) {
        perror("write");
        free(linebuf);
        free(enc_buf);
        break;
      }

      free(linebuf);
      free(enc_buf);
    }

    if (pfds[1].revents & POLLIN) {
      uint8_t header_buf[8];
      ssize_t n = read_exact(sockfd, header_buf, sizeof(header_buf));
      if (n != sizeof(header_buf)) {
        if (n < 0)
          perror("read");
        break;
      }
      uint32_t sender_id = be32_to_u32(header_buf);
      uint32_t len = be32_to_u32(header_buf + 4);

      char *enc_buf = malloc(len);
      n = read_exact(sockfd, enc_buf, len);
      if (n <= 0 || n < (ssize_t)len) {
        free(enc_buf);
        break;
      }

      static const uint8_t key[32] = {
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
          0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
          0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
      static const uint8_t iv[16] = {0};
      AesContext actx;
      aes_init(&actx, KEY_TYPE_AES256, key);

      uint8_t *buf = malloc(len + 1);
      aes_ctr_xcrypt(&actx, len, buf, (const uint8_t *)enc_buf, NULL, iv);
      buf[len] = '\0';
      free(enc_buf);

      char *final_str = NULL;
      ssize_t nb = asprintf(&final_str, "#%d> %s\n", sender_id, buf);
      free(buf);
      if (nb == -1) {
        perror("asprintf");
        continue;
      }
      write(1, final_str, strlen(final_str)); // write to stdout
      free(final_str);
    }
  }

  close(sockfd);
  return 0;
}
