#ifndef _STR_H_
#define _STR_H_

#include <stdlib.h>

typedef struct {
    char first_char;
} stringDataSegment_t;

typedef struct {
    stringDataSegment_t *ptr;
} string;

typedef struct {
    size_t allocated_bytes;
    size_t length;
    char data[];
} stringHeader_t;

#define string(input)                                               \
    _Generic((typeof(input) _input = input),                        \
        char *: stringFromPtr(_input),                              \
        string: stringFromString(_input),                           \
        default: static_assert(0, "type must be string or char *"),  \
    )           

string stringFromCharPtr(char *);
string stringFromString(string);

#endif // _STR_H