#include "parser.h"
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  SET,
  HASH_SET,
  HASH_GET,
  GET,
  HASH_REMOVE,
  REMOVE,
} RequestType;

RequestType *request_type_from_req(char *req) {
  RequestType *rt = malloc(sizeof(RequestType));
  if (!rt) {
    perror("failed allocate memory for request type");

    return NULL;
  }

  if (strcmp(req, "get") == 0) {
    *rt = GET;
  } else if (strcmp(req, "set") == 0) {
    *rt = SET;
  } else if (strcmp(req, "remove") == 0) {
    *rt = REMOVE;
  } else {
    printf("ERROR unknown request type: %s", req);
    return NULL;
  }

  return rt;
}

Result *route_remove_request(Storage *storage, char *key) {
  Result *res = malloc(sizeof(Result));
  if (!res) {
    perror("failed allocate memory for request");

    return NULL;
  }

  res->message = calloc(32, sizeof(char));
  if (!res->message) {
    perror("failed allocate memory for message");

    free(res);

    return NULL;
  }

  int status = storage_remove(storage, key);
  if (status != 0) {
    strcpy(res->message, "failed remove from storage");
  } else {
    strcpy(res->message, "success");
  }

  return res;
}

Result *route_get_request(Storage *storage, char *key) {
  Result *res = malloc(sizeof(Result));
  if (!res) {
    perror("failed allocate memory for request");

    return NULL;
  }

  res->value = NULL;

  res->message = calloc(32, sizeof(char));

  DataObject *obj = storage_get(storage, key);
  if (obj == NULL) {
    strcpy(res->message, "failed find value in cash");
  } else {
    res->value = obj;

    strcpy(res->message, "success");
  }

  return res;
}

Result *route_set_request(Storage *storage, char *key, void *value) {
  Result *res = malloc(sizeof(Result));
  if (!res) {
    perror("failed allocate memory for request");

    return NULL;
  }

  res->message = calloc(32, sizeof(char));
  if (!res->message) {
    perror("failed allocate memory for res message");
    free(res);

    return NULL;
  }

  DataObject *obj = malloc(sizeof(DataObject));
  obj->type = STRING;
  obj->value = value;

  int status = storage_set(storage, key, obj);
  if (status != 0) {
    perror("failed set value");

    return NULL;
  }

  return res;
}

Result *route_hset_request(Storage)

Result *route_request(char *request) {
  char *token;
  char *saveptr;

  token = strtok_r(request, " ", &saveptr);
  char *func = strdup(token);

  RequestType *rt = request_type_from_req(request);
  if (rt == NULL) {
    perror("failed parse request type");

    return NULL;
  }

  token = strtok_r(NULL, " ", &saveptr);
  char *key = strdup(token);
  if (key == NULL) {
    perror("failed parse key");

    return NULL;
  }

  char *args = NULL;

  if (saveptr && *saveptr != '\0') {
    args = strdup(saveptr);
  } else {
    return NULL;
  }
}
