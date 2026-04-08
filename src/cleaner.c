#include "cleaner.h"
#include "storage.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct CleanerArgs {
  Storage **storage;
  int duration;
  int should_exit;
};

static void *clean(void *arg) {
  struct CleanerArgs *args = (struct CleanerArgs *)arg;

  Storage *storage = *(args->storage);

  while (1) {
    storage_destroy(*(args->storage));

    Storage *new_storage = create_storage(storage->capacity);
    if (args->storage == NULL) {
      perror("failed create new storage");

      pthread_exit(NULL);
    }

    pthread_mutex_lock(&mutex);

    *(args->storage) = new_storage;

    pthread_mutex_unlock(&mutex);

    sleep(args->duration);
  }

  pthread_exit(NULL);
}

void start_clean(Storage **storage, int duration) {
  pthread_t cleaner_thread;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  size_t stack = 2 * 1024 * 1024;
  pthread_attr_setstacksize(&attr, stack);

  struct CleanerArgs *args = malloc(sizeof(struct CleanerArgs));
  if (!args) {
    perror("failed allocate memory for args");
    pthread_attr_destroy(&attr);
    return;
  }

  args->duration = duration;
  args->storage = storage;
  args->should_exit = 0;

  int res = pthread_create(&cleaner_thread, &attr, clean, args);
  if (res != 0) {
    perror("failed create thread");
    free(args);
    pthread_attr_destroy(&attr);
    return;
  }

  pthread_attr_destroy(&attr);
}