#ifndef STORAGE_H
#define STORAGE_H

#include "data_types.h"
#include <pthread.h>
#include <stddef.h>

extern pthread_mutex_t mutex;

typedef struct StorageEntry {
  char *key;
  DataObject *value;
  struct StorageEntry *next;
} StorageEntry;

typedef struct {
  StorageEntry **buckets;
  size_t capacity;
  size_t size;
  float max_load;
} Storage;

Storage *create_storage(size_t initial_capacity);
void storage_destroy(Storage *storage);

int storage_set(Storage *storage, const char *key, DataObject *value);
DataObject *storage_get(Storage *storage, const char *key);
int storage_remove(Storage *storage, const char *key);

size_t storage_size(const Storage *storage);
int storage_print_stats(const Storage *storage);

#endif
