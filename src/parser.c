#include "parser.h"
#include "data_types.h"
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
  UNKNOWN,
} RequestType;

RequestType request_type_from_req(char *req) {
  RequestType rt;

  if (strcmp(req, "get") == 0) {
    rt = GET;
  } else if (strcmp(req, "set") == 0) {
    rt = SET;
  } else if (strcmp(req, "remove") == 0) {
    rt = REMOVE;
  } else if (strcmp(req, "hset") == 0) {
    rt = HASH_SET;
  } else if (strcmp(req, "hget") == 0) {
    rt = HASH_GET;
  } else if (strcmp(req, "hremove") == 0) {
    rt = HASH_REMOVE;
  } else {
    printf("ERROR unknown request type: %s", req);

    rt = UNKNOWN;
  }

  return rt;
}

Result *route_remove_request(Storage *storage, char *key) {
  int status = storage_remove(storage, key);
  if (status != 0) {
    ERROR_RESULT("failed remove from storage");
  } else {
    SUCCESS_RESULT(NULL)
  }
}

Result *route_get_request(Storage *storage, char *key) {
  DataObject *obj = storage_get(storage, key);
  if (obj == NULL) {
    ERROR_RESULT("failed fetch object from storage");
  } else {
    SUCCESS_RESULT(obj);
  }
}

Result *route_set_request(Storage *storage, char *key, void *value) {
  DataObject *obj = malloc(sizeof(DataObject));
  if (!obj) {
    ERROR_RESULT("failed create DataObject");
  }

  obj->type = STRING;
  obj->value = value;

  int status = storage_set(storage, key, obj);
  if (status != 0) {
    ERROR_RESULT("failed set object in storage");
  }

  SUCCESS_RESULT(obj);
}

Result *route_hset_request(Storage *storage, char *storage_key, char *h_key,
                           String value) {
  DataObject *hash_table = storage_get(storage, storage_key);

  Hash *hash;

  if (hash_table == NULL) {
    hash = create_hash_table();
    if (hash == NULL) {
      ERROR_RESULT("failed create hash table");
    }
  } else {
    if (hash_table->type != HASH) {
      ERROR_RESULT("hash table object type is not HASH");
    }

    hash = (Hash *)hash_table->value;
  }

  int status = hash_set(hash, h_key, value);
  if (status != 0) {
    ERROR_RESULT("failed set value in hash");
  }

  hash_table->value = (void *)hash;

  status = storage_set(storage, storage_key, hash_table);
  if (status != 0) {
    ERROR_RESULT("failed set obj in storage");
  }

  SUCCESS_RESULT(NULL)
}

Result *route_hget_request(Storage *storage, char *key, char *hkey) {
  DataObject *hash_table_obj = storage_get(storage, key);
  if (hash_table_obj == NULL) {
    ERROR_RESULT("failed allocate");
  }

  if (hash_table_obj->type == HASH) {
    ERROR_RESULT("obj type is not hash");
  }

  Hash *hash = (Hash *)hash_table_obj->value;
  String value = hash_get(hash, hkey);
  if (value == NULL) {
    ERROR_RESULT("failed get from hash");
  }

  DataObject *obj = create_data_object(STRING, (void *)value);
  if (!obj) {
    ERROR_RESULT("failed create data object");
  }

  SUCCESS_RESULT(obj)
}

Result *route_hremove_request(Storage *storage, char *key, char *hkey) {
  DataObject *hash_table_obj = storage_get(storage, key);
  if (hash_table_obj == NULL) {
    ERROR_RESULT("failed allocate");
  }

  if (hash_table_obj->type == HASH) {
    ERROR_RESULT("obj type is not hash");
  }

  Hash *hash = (Hash *)hash_table_obj->value;
}

Result *route_request(Storage *storage, char *request) {
  char *token;
  char *saveptr;

  token = strtok_r(request, " ", &saveptr);
  char *func = strdup(token);

  RequestType rt = request_type_from_req(request);
  if (rt == UNKNOWN) {
    ERROR_RESULT("unknown request type");
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

  switch (rt) {
  case REMOVE:
    return route_remove_request(storage, key);
  case GET:
    return route_get_request(storage, key);
  case SET:
    return route_set_request(storage, key, args);
  case HASH_SET:
    saveptr = NULL;

    token = strtok_r(args, " ", &saveptr);
    char *hkey = strdup(token);

    token = strtok_r(NULL, " ", &saveptr);
    char *hvalue = strdup(token);

    return route_hset_request(storage, key, hkey, hvalue);
  case HASH_GET:
    return route_hget_request(storage, key, args);
  case HASH_REMOVE:

  case UNKNOWN:
    ERROR_RESULT("unknown operation type");
  }
}
