#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
    ERROR,
    MESSAGE,
    WARN
}LogType;


typedef struct{
    char key[20];
    char *value;
}LogAttr;

typedef struct{
    LogAttr *attrs;
    size_t len;
}Attrs;

Attrs *new_attrs();
void add_attr(Attrs *attrs, char key[20], char *value);

void log_in(FILE *dest, char *message, Attrs *attrs);

