#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "parser.h"
#include "server.h"

#define MAX_CLIENTS 8

Server *new_server(Storage *storage, size_t buf_size, int port, char *addr) {
  printf("INFO create new server size: %zu, port: %d, addr: %s", buf_size, port,
         addr);

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

int start_server(Server *server) {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  char buffer[server->buf_size];
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("ERROR failed create server socket");

    return 1;
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    printf("failed set socket options");

    return 1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server->port);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("failed socket to addr");

    return 1;
  }

  if (listen(server_fd, MAX_CLIENTS) < 0) {
    printf("failed listen");

    return 1;
  }

  printf("Server started on port: %d, waiting connections...", server->port);

  while (1) {
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      printf("failed accept client conn");
      continue;
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    while (1) {
      ssize_t bytes = recv(client_fd, buffer, server->buf_size - 1, 0);
      if (bytes <= 0) {
        if (bytes == 0)
          printf("Client disconnected\n");
        else
          printf("failed reciev");
        break;
      }

      buffer[bytes] = '\0';
      printf("Recieved: %s", buffer);

      route_request(server->storage, buffer);

      send(client_fd, buffer, bytes, 0);
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
