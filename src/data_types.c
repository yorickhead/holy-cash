#include "data_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_HASH_LENGTH 16

DataObject *create_data_object(DataType type, void *value) {
  DataObject *obj = malloc(sizeof(DataObject));
  if (!obj) {
    perror("failed allocate memory for data object");

    return NULL;
  }

  obj->type = type;
  obj->value = value;

  return obj;
}

void destroy_data_object(DataObject *obj) {
  free(obj->value);

  free(obj);
}

Hash *create_hash_table(int size) {
  Hash *table = malloc(sizeof(Hash));
  if (!table) {
    perror("failed allocate memory for table");

    return NULL;
  }

  table->pairs = calloc(DEFAULT_HASH_LENGTH, sizeof(KeyValuePair));
  if (!table->pairs) {
    perror("failed allocate memory for table pairs");

    return NULL;
  }

  return table;
}

static int exist_in_hash(Hash *table, char *key) {
  for (int i = 0; i < DEFAULT_HASH_LENGTH; i++) {
    if (strcmp(table->pairs[i].key, key)) {
      return i;
    }
  }

  return 1;
}

int hash_set(Hash *table, char *key, DataObject *obj) {

}