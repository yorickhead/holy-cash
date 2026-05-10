#include "logger.h"
#include <stdio.h>
#include <string.h>

static size_t attrs_size(Attrs *attrs) {
    LogAttr *current_attr = attrs->attrs;

    size_t size;

    for (int i = 0; i < attrs->len; i++) {
        if (current_attr == NULL) {
            break;
        }

        size += strlen(attrs->attrs->value);
        size += 4;
        
        current_attr++;
    }

    size += 7;

    return size;
}

static void route_attr(char **buf, LogAttr *attr){
    strcat(*buf, attr->key);
    strcat(*buf, " : ");
    strcat(*buf, attr->value);
    strcat(*buf, "; ");
}

static char *attrs_line(Attrs *attrs) {
    if (attrs == NULL) {
        perror("attrs is null");
    }

    LogAttr *current_attr = attrs->attrs;
    
    size_t size = attrs_size(attrs);

    char *buf = calloc(size, sizeof(char));
    if (buf == NULL) {
        perror("failed allocate memory for buf");

        return NULL;
    }
    buf[size] = '\0';

    for (int i = 0; i < attrs->len; i++) {
        if (current_attr == NULL) {
            break;
        }

        route_attr(&buf, current_attr);
    }

    return buf;
}

void log_in(FILE *dest, char *message, Attrs *attrs){
    if (dest == NULL) {
        perror("dest or attr is null");

        return;
    }

    char *attrs_buf = attrs_line(attrs);
    size_t msg_len = strlen(message);
    size_t attrs_len = strlen(attrs_buf);


}