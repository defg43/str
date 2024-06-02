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

string stringFromCharPtr(const char *);
string stringFromString(string);
void destroyString(string input);

#endif // _STR_H