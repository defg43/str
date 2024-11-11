#ifndef _STR_H_
#define _STR_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte;
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

#define array(type) struct array_##type { type *element; size_t count; }

#define push(arr, elem) ({                                                  \
    auto _arr = &(arr);                                                     \
    auto _elem = (elem);                                                    \
    _arr->element =                                                         \
        realloc(_arr->element, sizeof(_arr->element) * (_arr->count + 1));  \
    printf("pushing element to array\n");                                   \
    _arr->element[_arr->count] = _elem;                                     \
    _arr->count++;                                                          \
})

#define pop(arr) ({                                                         \
    auto _arr = &(arr);                                                     \
    auto ret = _arr->element[_arr->count - 1];                              \
    _arr->count--;                                                          \
    _arr->element =                                                         \
        realloc(_arr->element, sizeof(_arr->element) * _arr->count);        \
    ret;                                                                    \
})


string stringFromCharPtr(const char *);
string stringFromString(string);
stringHeader_t *getHeaderPointer(string);
void destroyString(string);
size_t stringlen(string);
size_t stringbytesalloced(string);

int stringcmp(string, string);
int stringncmp(string, string, size_t);
bool stringeql(string, string);
bool stringneql(string, string, size_t);
bool streql(char *, char *);
bool strneql(char *, char *, size_t);
bool stringeqlidx(string, size_t, string);

// those allocate a new string
string sliceFromString(string, size_t, size_t);
string sliceFromCharPtr(const char *, size_t, size_t);
string concat(string, string);

// these operate on the existing string
string stringReverse(string);

// these modify the original buffer
string stringGrowBuffer(string, size_t);
string appendCharPtr(string, const char *);
string appendString(string, string);
string prependCharPtr(string, const char *);
string prepend(string, string);
// string stringUppercase(string);
// string stringUppercaseRange(string, size_t, size_t);
// string stringLowercase(string);
// string stringLowercaseRange(string, size_t, size_t);

bool binaryPrefix(unsigned char, unsigned char, size_t);

array(string) tokenizeString(char *, char *);

#define tokenize(str, delim) tokenizeString(coerce(str, char *), coerce(delim, char *))
#define append(str, to_append_)                                                         \
    ({                                                                                  \
        auto to_append = (to_append_);                                                  \
        _Generic((to_append),                                                           \
            const char *: appendCharPtr(str, coerce(to_append, const char *)),          \
            char *:       appendCharPtr(str, coerce(to_append, char *)),                \
            string:       appendString(str, coerce(to_append, string))                  \
        );                                                                              \
    })
#define prepend(str, to_prepend_)                                                       \
    ({                                                                                  \
        auto to_prepend = (to_prepend_);                                                \
        _Generic((to_prepend),                                                          \
            const char *: prependCharPtr(str, coerce(to_prepend, const char *)),        \
            char *:       prependCharPtr(str, coerce(to_prepend, char *)),              \
            string:       prependString(str, coerce(to_prepend, string))                \
        );                                                                              \
    })
#endif // _STR_H_
