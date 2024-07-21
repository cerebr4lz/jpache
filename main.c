/* jpache.c */

#include <arpa/inet.h>
#include <complex.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define LISTENADDR "127.0.0.1"
// #define PORT 42069

/* initialize server: exits on failure (sets errno), else returns socket fd */
int srvr_init(int port) {
  int sckt;
  struct sockaddr_in srvr;
  // NOTE: local struct variable, may need to make global
  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if (sckt < 0) {
    fprintf(stderr, "ERROR[socket()]: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  srvr.sin_family = AF_INET;
  srvr.sin_addr.s_addr = inet_addr(LISTENADDR);
  srvr.sin_port = htons(port);
  int conn = bind(sckt, (struct sockaddr *)&srvr, sizeof(srvr));
  if (conn < 0) {
    fprintf(stderr, "ERROR[bind()]: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  if (listen(sckt, 42) < 0) {
    fprintf(stderr, "ERROR[listen()]: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  return sckt;
}

int client_accept(int sckt) {
  int clientfd;
  socklen_t addrlen;
  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));
  clientfd = accept(sckt, (struct sockaddr *)&client, &addrlen);
  if (clientfd < 0) {
    fprintf(stderr, "ERROR: Unable to accept socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  return clientfd;
}

void client_connect(int sckt, int clnt) {
  (void)sckt;
  (void)clnt;
}

int main(int argc, char **argv) {
  printf("hello, World!\n");
  // handle args ?

  int sckt, clnt;
  char *port;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <listening port>\n", argv[0]);
    return EXIT_FAILURE;
  }
  port = argv[1];
  // TODO: atoi() deprecated, use strtol() instead. [or make my own]
  sckt = srvr_init(atoi(port));
  if (!sckt) {
    fprintf(stderr, "ERROR: Unable to initialize server: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  printf("Listening on %s:%s\n", LISTENADDR, port);
  while (true) {
    clnt = client_accept(sckt);
    if (clnt < 0) {
      fprintf(stderr, "ERROR: Connection Refused (client): %s\n",
              strerror(errno));
      continue;
    }
    printf("Incoming Connection...");
    pid_t forked = fork();
    if (forked != 0) {
      fprintf(stderr, "ERROR: Process Unforked: %s\n", strerror(errno));
      return EXIT_FAILURE;
      client_connect(sckt, clnt);
    }
  }
  fprintf(stderr, "ERROR: Unreachable: %s\n", strerror(errno));
  return EXIT_FAILURE;
}
