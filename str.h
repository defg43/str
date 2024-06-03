#ifndef _STR_H_
#define _STR_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char first_char;
} dataSegmentOfString_t;

typedef uint32_t utf32_t;

typedef struct {
    union {
        dataSegmentOfString_t *data;
        char *at;
        utf32_t *as_utf32;
    };
} string;

typedef struct {
    size_t allocated_bytes;
    size_t length;
    char data[];
} stringHeader_t;

typedef struct {
    union {
        void *data;
        void *at;
    };
    size_t allocated_bytes;
    size_t element_size;
    size_t length;
} array_t;

#define coerce(_expr, type) ({                                              \
    auto expr = (_expr);                                                    \
    union {                                                                 \
        typeof(expr) i_have_this;                                           \
        type but_i_want_this;                                               \
    } converter = { .i_have_this = expr };                                  \
    converter.but_i_want_this;                                              \
})

#define string(input)                                                       \
    ({                                                                      \
        auto _input = (input);                                              \
        static_assert( _Generic((_input),                                   \
            char *: true,                                                   \
            const char *: true,                                             \
            string: true,                                                   \
            default: false                                                  \
        ), "input must be char * or string");                               \
        _Generic((_input),                                                  \
            char *: stringFromCharPtr(coerce(_input, char *)),              \
            const char *: stringFromCharPtr(coerce(_input, const char *)),  \
            string: stringFromString(coerce(_input, string))                \
        );                                                                  \
    })

#define array(type) struct array##type { type *arr; size_t count; }

array(string) tokenizeString(string, string);

string stringFromCharPtr(const char *);
string stringFromString(string);
void destroyString(string);
string concat(string, string);
stringHeader_t *getHeaderPointer(string);
int stringcmp(string, string);
int stringncmp(string, string, size_t);
bool stringeql(string, string);
bool stringneql(string, string, size_t);
size_t stringlen(string);
size_t stringbytesalloced(string);
bool stringeqlidx(string, size_t, string);
string stringReverse(string);

#endif // _STR_H