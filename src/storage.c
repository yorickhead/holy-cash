#include "storage.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long hash(const unsigned char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

Storage *create_storage(size_t capacity) {
  if (capacity < 8)
    capacity = 8;

  Storage *storage = malloc(sizeof(Storage));
  if (!storage) {
    perror("Failed to allocate Storage");
    return NULL;
  }

  storage->capacity = capacity;
  storage->size = 0;
  storage->max_load = 0.75f;

  storage->buckets = calloc(capacity, sizeof(StorageEntry *));
  if (!storage->buckets) {
    perror("Failed to allocate buckets");
    free(storage);
    return NULL;
  }

  return storage;
}

// Helper to find entry - assumes mutex is already held by caller
static StorageEntry **find_entry_unlocked(Storage *storage, const char *key) {
  unsigned long h = hash((const unsigned char *)key) % storage->capacity;
  StorageEntry **entry = &storage->buckets[h];

  while (*entry) {
    if (strcmp((*entry)->key, key) == 0)
      return entry;
    entry = &(*entry)->next;
  }

  return entry;
}
static void resize(Storage *storage) {
  pthread_mutex_lock(&mutex);

  size_t old_capacity = storage->capacity;
  StorageEntry **old_buckets = storage->buckets;

  storage->capacity *= 2;
  storage->buckets = calloc(storage->capacity, sizeof(StorageEntry *));
  if (!storage->buckets) {
    storage->buckets = old_buckets;
    storage->capacity = old_capacity;
    fprintf(stderr, "Warning: resize failed, continuing with old size\n");
    return;
  }

  storage->size = 0;

  for (size_t i = 0; i < old_capacity; i++) {
    StorageEntry *entry = old_buckets[i];
    while (entry) {
      StorageEntry *next = entry->next;

      unsigned long h =
          hash((const unsigned char *)entry->key) % storage->capacity;
      entry->next = storage->buckets[h];
      storage->buckets[h] = entry;

      storage->size++;
      entry = next;
    }
  }

  pthread_mutex_unlock(&mutex);

  free(old_buckets);
}

int storage_set(Storage *storage, const char *key, DataObject *value) {
  pthread_mutex_lock(&mutex);

  if (!storage || !key) {
    pthread_mutex_unlock(&mutex);
    return 1;
  }

  if ((float)(storage->size + 1) / storage->capacity > storage->max_load) {
    resize(storage);
  }

  StorageEntry **entry_ptr = find_entry_unlocked(storage, key);

  if (*entry_ptr) {
    free((*entry_ptr)->value);
    (*entry_ptr)->value = value;
  } else {
    StorageEntry *new_entry = malloc(sizeof(StorageEntry));
    if (!new_entry) {
      perror("Failed to allocate new entry");
      return 1;
    }

    new_entry->key = strdup(key);
    if (!new_entry->key) {
      free(new_entry);
      return 1;
    }

    new_entry->value = value;
    new_entry->next = *entry_ptr;
    *entry_ptr = new_entry;

    storage->size++;
  }

  pthread_mutex_unlock(&mutex);

  return 0;
}

DataObject *storage_get(Storage *storage, const char *key) {
  pthread_mutex_lock(&mutex);

  if (!storage || !key) {
    pthread_mutex_unlock(&mutex);
    return NULL;
  }

  StorageEntry **entry_ptr = find_entry_unlocked(storage, key);
  DataObject *result = (*entry_ptr) ? (*entry_ptr)->value : NULL;

  pthread_mutex_unlock(&mutex);

  return result;
}

int storage_remove(Storage *storage, const char *key) {
  pthread_mutex_lock(&mutex);

  if (!storage || !key) {
    pthread_mutex_unlock(&mutex);
    return 0;
  }

  StorageEntry **entry_ptr = find_entry_unlocked(storage, key);
  if (!*entry_ptr) {
    pthread_mutex_unlock(&mutex);
    return 0;
  }

  StorageEntry *to_delete = *entry_ptr;
  *entry_ptr = to_delete->next;

  free(to_delete->key);
  free(to_delete);

  storage->size--;

  pthread_mutex_unlock(&mutex);
  return 1;
}

void storage_destroy(Storage *storage) {
  pthread_mutex_lock(&mutex);

  if (!storage)
    return;

  for (size_t i = 0; i < storage->capacity; i++) {
    StorageEntry *entry = storage->buckets[i];
    while (entry) {
      StorageEntry *next = entry->next;
      free(entry->key);
      free(entry->value);
      free(entry);
      entry = next;
    }
  }

  free(storage->buckets);
  free(storage);

  pthread_mutex_unlock(&mutex);
}
