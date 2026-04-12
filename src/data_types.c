// data_types.c
#include "data_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_HASH_SIZE 16
#define LOAD_FACTOR 0.75

// ====================== DataObject ======================

DataObject *create_data_object(DataType type, void *value) {
  DataObject *obj = malloc(sizeof(DataObject));
  if (!obj) {
    perror("failed to allocate memory for data object");
    return NULL;
  }

  obj->type = type;
  obj->value = value;
  return obj;
}

void destroy_data_object(DataObject *obj) {
  if (!obj)
    return;

  if (obj->value) {
    if (obj->type == STRING) {
      free(obj->value);
    } else if (obj->type == HASH) {
      destroy_hash((Hash *)obj->value);
    }
  }
  free(obj);
}

// ====================== Hash Table ======================

Hash *create_hash_table(void) {
  Hash *table = malloc(sizeof(Hash));
  if (!table) {
    perror("failed to allocate memory for hash table");
    return NULL;
  }

  table->size = INITIAL_HASH_SIZE;
  table->count = 0;
  table->pairs = calloc(table->size, sizeof(KeyValuePair));
  if (!table->pairs) {
    perror("failed to allocate memory for hash pairs");
    free(table);
    return NULL;
  }

  return table;
}

static unsigned int hash_function(const char *key) {
  unsigned int hash = 0;
  while (*key) {
    hash = hash * 31 + (unsigned char)*key++;
  }
  return hash;
}

static void resize_hash_table(Hash *table) {
  int old_size = table->size;
  KeyValuePair *old_pairs = table->pairs;

  table->size *= 2;
  table->pairs = calloc(table->size, sizeof(KeyValuePair));
  if (!table->pairs) {
    perror("failed to resize hash table");
    table->pairs = old_pairs;
    table->size = old_size;
    return;
  }

  table->count = 0;

  for (int i = 0; i < old_size; i++) {
    if (old_pairs[i].key != NULL) {
      hash_set(table, old_pairs[i].key, old_pairs[i].value);
    }
  }

  free(old_pairs);
}

int hash_set(Hash *table, char *key, String value) {
  if (!table || !key)
    return -1;

  if ((float)table->count / table->size > LOAD_FACTOR) {
    resize_hash_table(table);
  }

  unsigned int index = hash_function(key) % table->size;
  unsigned int start_index = index;

  do {
    if (table->pairs[index].key == NULL) {
      table->pairs[index].key = strdup(key);
      table->pairs[index].value = value;
      table->count++;
      return 0;
    }

    if (strcmp(table->pairs[index].key, key) == 0) {
      free(table->pairs[index].value);
      table->pairs[index].value = value;
      return 0;
    }

    index = (index + 1) % table->size;
  } while (index != start_index);

  return -1;
}

String hash_get(Hash *table, const char *key) {
  if (!table || !key)
    return NULL;

  unsigned int index = hash_function(key) % table->size;
  unsigned int start_index = index;

  do {
    if (table->pairs[index].key == NULL) {
      return NULL; // не найдено
    }

    if (strcmp(table->pairs[index].key, key) == 0) {
      return table->pairs[index].value;
    }

    index = (index + 1) % table->size;
  } while (index != start_index);

  return NULL;
}

void destroy_hash(Hash *table) {
  if (!table)
    return;

  for (int i = 0; i < table->size; i++) {
    if (table->pairs[i].key) {
      free(table->pairs[i].key);
      free(table->pairs[i].value);
    }
  }

  free(table->pairs);
  free(table);
}
