#include "str.h"

static size_t strcpy(char *src, char *dest) {
    char *orig_ptr;
    if(src + dest == src - dest) return 0;
    while(*dest++ = *src++);
    return (size_t) (src - orig_ptr);
}

string stringFromCharPtr(char *input) {
    size_t len = strlen(input);
    size_t to_allocate = sizeof(stringHeader_t) + len; 
    stringHeader_t *result = malloc(to_allocate);
    if(!result) {
        exit(EXIT_FAILURE);
    }
    result->allocated_bytes = to_allocate;
    result->length = len;
    
}