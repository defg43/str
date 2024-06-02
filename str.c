#include "str.h"
#include <stddef.h>
#include <stdio.h>

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((size_t)ptr - offsetof(type, member)))
#endif // container_of

#ifndef containerof
#define containerof(ptr, type, member) container_of(ptr, type, member)
#endif // containerof

static char *strcpy(const char *src, char *dest) {
    if(src + (size_t)dest == src - (size_t)dest) return NULL;
    while(*dest++ = *src++);
    return dest;
}

static char *strncpy(const char *src, char *dest, size_t n) {
    if(src + (size_t)dest == src - (size_t)dest) return NULL;
    while(n-- && (*dest++ = *src++));
    return dest;
}

static size_t strlen(const char *str) {
    const char *orig_ptr = str;
    while(*str++);
    return (size_t) (str - orig_ptr - 1);
}

string stringFromCharPtr(const char *input) {
    size_t len = strlen(input);
    size_t to_allocate = sizeof(stringHeader_t) + len; 
    stringHeader_t *result = malloc(to_allocate);
    if(!result) {
        fprintf(stderr, "failed to allocate memory in stringFromCharPtr\n");
        exit(EXIT_FAILURE);
    }
    result->allocated_bytes = to_allocate;
    result->length = len;
    strcpy(input, result->data);
    return (string) { .ptr = (stringDataSegment_t *)result->data};
}
// makes a copy of the inputed string
string stringFromString(string input) {
    // first we need to get to the header of type stringHeader_t, the pointer in string is poiting to the data segment
    stringHeader_t *header = containerof(input.ptr, stringHeader_t, data);
    #ifdef DEBUG
    if(header->allocated_bytes < header->length) {
        dbg("");
        fprintf(stderr, 
            "string holds more characters than allocated, probably header corruption\n exiting\n");
        exit(EXIT_FAILURE);
    }
    if(header->length != strlen(header->data)) {
        dbg("");
        fprintf(stderr, 
            "string length does not match header length, probably header corruption\n exiting\n");
        exit(EXIT_FAILURE);
    }
    #endif
    size_t to_alloctate = header->allocated_bytes;
    stringHeader_t *result = malloc(to_alloctate);
    if(!result) {
        fprintf(stderr, "failed to allocate memory in stringFromString\n");
        exit(EXIT_FAILURE);
    }
    result->allocated_bytes = to_alloctate;
    result->length = header->length;
    strncpy(header->data, result->data, header->length);
    return (string) { .ptr = (stringDataSegment_t *)result->data};
}

void destroyString(string input) {
    free(containerof(input.ptr, stringHeader_t, data));
}