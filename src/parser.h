#ifndef PARSER_H
#define PARSER_H

#include "storage.h"
#include <stdbool.h>

typedef struct {
  char *message;
  DataObject *value;
} Result;

Result *route_request(char *request);

#endif
