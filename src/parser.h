#ifndef PARSER_H
#define PARSER_H

#define ERROR_RESULT(msg)                                                      \
  do {                                                                         \
    Result *res = malloc(sizeof(Result));                                      \
    if (!res)                                                                  \
      return NULL;                                                             \
    res->value = NULL;                                                         \
    if (asprintf(&res->message, "ERROR: %s", (msg)) == -1) {                   \
      free(res);                                                               \
      return NULL;                                                             \
    }                                                                          \
    return res;                                                                \
  } while (0)

#define SUCCESS_RESULT(obj)                                                    \
  {                                                                            \
    Result *res = malloc(sizeof(Result));                                      \
    res->message = calloc(8, sizeof(char));                                    \
    res->value = obj;                                                          \
    strcpy(res->message, "success");                                           \
    return res;                                                                \
  }

#include "storage.h"
#include <stdbool.h>

typedef struct {
  char *message;
  DataObject *value;
} Result;

Result *route_request(char *request);

#endif
