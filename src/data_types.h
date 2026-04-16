// data_types.h
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef enum {
  STRING,
  HASH,
  LIST,
} DataType;

typedef struct {
  DataType type;
  void *value;
} DataObject;

typedef char *String;

typedef struct {
  char *key;
  String value;
} KeyValuePair;

typedef struct {
  int size;
  int count;
  KeyValuePair *pairs;
} Hash;

Hash *create_hash_table(void);

int hash_set(Hash *table, char *key, String value);
String hash_get(Hash *table, const char *key);

void destroy_hash(Hash *hash);
int hash_set(Hash *hash, char *key, String value);

#endif
