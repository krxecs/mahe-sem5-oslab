#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <ay/aes.h>

#include "dbg.h"
#include "helper.h"
#include "vec.h"

void host_to_addrinfo(const char *host, const char *port,
                      struct addrinfo **res) {
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_flags = AI_PASSIVE};
  int status = getaddrinfo(host, port, &hints, res);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(1);
  }
}

void addrinfo_to_pfdvec(struct addrinfo *ai, void *vec) {
  for (struct addrinfo *p = ai; p != NULL; p = p->ai_next) {
    int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      perror("socket");
      continue;
    }
    int yes = 1;
    perrf(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1,
          "setsockopt");
    perrf(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1, "bind");
    perrf(listen(sockfd, 10) == -1, "listen");
    struct pollfd pfd = {.fd = sockfd, .events = POLLIN};
    ay_vec(struct pollfd) pfdvec;
    memcpy(&pfdvec, vec, sizeof pfdvec);
    ay_vec_push_back(&pfdvec, pfd);
    memcpy(vec, &pfdvec, sizeof pfdvec);
  }
}

void accept_new_connection(void *pollfds_void, void *client_ids_vec_void,
                           int *client_count, int listener_fd) {
  ay_vec(struct pollfd) *pollfds = pollfds_void;
  ay_vec(int) *client_ids_vec = client_ids_vec_void;

  struct sockaddr_storage client_addr;
  socklen_t addr_len = sizeof(client_addr);
  int client_fd =
      accept(listener_fd, (struct sockaddr *)&client_addr, &addr_len);

  perrf(client_fd == -1, "accept");
  struct pollfd pfd = {.fd = client_fd, .events = POLLIN};

  ay_vec_push_back(pollfds, pfd);
  ay_vec_push_back(client_ids_vec, ++(*client_count));
  fprintf(stderr, "New client #%d connected\n", *client_count);
}

void read_from_client(void *pollfds_void, void *client_ids_vec_void,
                      size_t client_index, size_t num_listeners) {
  ay_vec(struct pollfd) *pollfds = pollfds_void;
  ay_vec(int) *client_ids_vec = client_ids_vec_void;

  uint8_t lenbuf[4];
  ssize_t n = read_exact(pollfds->data[client_index + num_listeners].fd, lenbuf,
                         sizeof(lenbuf));
  if (n <= 0 || n < (ssize_t)sizeof(lenbuf)) {
    if (n < 0)
      perror("read");
    close(pollfds->data[client_index + num_listeners].fd);
    pollfds->data[client_index + num_listeners].fd = -1;
    ay_vec_delete(pollfds, client_index + num_listeners);
    ay_vec_delete(client_ids_vec, client_index + num_listeners);
    fprintf(stderr, "Client disconnected\n");
    return;
  }

  uint32_t msg_len = be32_to_u32(lenbuf);

  uint8_t *enc_buf = malloc(msg_len);
  n = read_exact(pollfds->data[client_index + num_listeners].fd, enc_buf,
                 msg_len);
  if (n <= 0 || n < (ssize_t)msg_len) {
    if (n < 0)
      perror("read");
    close(pollfds->data[client_index + num_listeners].fd);
    pollfds->data[client_index + num_listeners].fd = -1;
    ay_vec_delete(pollfds, client_index + num_listeners);
    ay_vec_delete(client_ids_vec, client_index + num_listeners);
    fprintf(stderr, "Client disconnected\n");
    free(enc_buf);
    return;
  }

  AesContext actx;
  static const uint8_t key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
  const uint8_t iv[16] = {0};
  aes_init(&actx, KEY_TYPE_AES256, key);

  uint8_t *buf = malloc(msg_len + 1);
  aes_ctr_xcrypt(&actx, msg_len, buf, enc_buf, NULL, iv);
  buf[msg_len] = '\0';
  free(enc_buf);

  uint32_t sender_id = client_ids_vec->data[client_index];

  uint8_t header_buf[8];
  u32_to_be32(header_buf, sender_id);
  u32_to_be32(header_buf + 4, (uint32_t)msg_len);

  // Broadcast to all other clients
  for (size_t j = num_listeners; j < pollfds->base.size; j++) {
    if (j != client_index + num_listeners && pollfds->data[j].fd >= 0) {
      uint8_t *enc_to_send = malloc(msg_len);
      aes_ctr_xcrypt(&actx, msg_len, enc_to_send, buf, NULL, iv);

      struct iovec iov[] = {
          {.iov_base = header_buf, .iov_len = sizeof(header_buf)},
          {.iov_base = enc_to_send, .iov_len = msg_len}};

      if (writev(pollfds->data[j].fd, iov, 2) == -1)
        perror("writev");

      free(enc_to_send);
    }
  }

  free(buf);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
    return 1;
  }

  struct addrinfo *ai;
  host_to_addrinfo(argv[1], argv[2], &ai);

  ay_vec(struct pollfd) vec;
  ay_vec_init(&vec);
  int client_count = 0;
  ay_vec(int) client_ids_vec;
  ay_vec_init(&client_ids_vec);
  addrinfo_to_pfdvec(ai, &vec);

  freeaddrinfo(ai);

  size_t num_listeners = vec.base.size;

  for (;;) {
    int poll_count = poll(vec.data, vec.base.size, -1);
    if (poll_count == -1) {
      perror("poll");
      break;
    }
    for (size_t i = 0; i < vec.base.size; i++) {
      if (vec.data[i].fd < 0)
        continue;
      if (vec.data[i].revents & POLLIN) {
        if (i < num_listeners)
          accept_new_connection(&vec, &client_ids_vec, &client_count,
                                vec.data[i].fd);
        else
          read_from_client(&vec, &client_ids_vec, i - num_listeners,
                           num_listeners);
      }
    }
  }

  // Cleanup
  for (size_t i = 0; i < vec.base.size; i++) {
    if (vec.data[i].fd >= 0)
      close(vec.data[i].fd);
  }
  ay_vec_free(&vec);
  ay_vec_free(&client_ids_vec);
  return 0;
}
