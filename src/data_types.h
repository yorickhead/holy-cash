typedef enum {
    STRING,
    HASH,
    LIST,
}DataType;

typedef struct {
    DataType type;
    void* value;
}DataObject;

typedef struct {
    int size;
    DataObject* elements;
}List;

typedef struct {
    char* key;
    DataObject value;
}KeyValuePair;

typedef struct {
    int size;
    KeyValuePair* pairs;
}Hash;

typedef char* String;