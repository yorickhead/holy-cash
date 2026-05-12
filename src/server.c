#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "server.h"

Server *new_server(Storage *storage, size_t buf_size, int port, char *addr) {
  printf("INFO create new server size: %zu, port: %d, addr: %s", buf_size, port, addr);

  Server *server = malloc(sizeof(Server));
  if (!server) {
    printf("ERROR failed allocate memory for server");

    return NULL;
  }

  size_t size = strlen(addr);

  server->addr = strdup(addr);
  server->buf_size = buf_size;
  server->port = port;
  server->storage = storage;

  return server;
}
