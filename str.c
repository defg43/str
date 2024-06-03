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
    return (string) { .data = (dataSegmentOfString_t *)result->data };
}
// makes a copy of the inputed string
string stringFromString(string input) {
    // first we need to get to the header of type stringHeader_t, the pointer in string is poiting to the data segment
    stringHeader_t *header = containerof(input.data, stringHeader_t, data);
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
    return (string) { .data = (dataSegmentOfString_t *)result->data };
}

void destroyString(string input) {
    free(containerof(input.data, stringHeader_t, data));
}

string concat(string a, string b) {
    stringHeader_t *a_header = containerof(a.data, stringHeader_t, data);
    stringHeader_t *b_header = containerof(b.data, stringHeader_t, data);
    size_t to_alloctate = a_header->allocated_bytes + b_header->allocated_bytes;
    stringHeader_t *result = malloc(to_alloctate);
    if(!result) {
        fprintf(stderr, "failed to allocate memory in concat\n");
        exit(EXIT_FAILURE);
    }
    result->allocated_bytes = to_alloctate;
    result->length = a_header->length + b_header->length;
    strncpy(a_header->data, result->data, a_header->length);
    strncpy(b_header->data, result->data + a_header->length, b_header->length);
    return (string) { .data = (dataSegmentOfString_t *)result->data };
}

stringHeader_t *getHeaderPointer(string input) {
    return containerof(input.data, stringHeader_t, data);
}

array(string) tokenizeString(string input, string delimiter) {
    for(size_t i = 0; i < stringlen(input); i++) {  

        
    }
}

int stringcmp(string a, string b) {
    stringHeader_t *a_header = getHeaderPointer(a);
    stringHeader_t *b_header = getHeaderPointer(b);
    if(a_header->length == 0 || b_header->length == 0) {
        return 0;
    }
    size_t min = a_header->length < b_header->length ? a_header->length : b_header->length;
    for(size_t i = 0; i < min; i++) {
        if(a_header->data[i] != b_header->data[i]) {
            return (int) a_header->data[i] - (int) b_header->data[i];
        }
    }
    return a_header->length - b_header->length;
}

int stringncmp(string a, string b, size_t n) {
    stringHeader_t *a_header = getHeaderPointer(a);
    stringHeader_t *b_header = getHeaderPointer(b);
    if(a_header->length == 0 || b_header->length == 0) {
        return 0;
    }
    size_t min = a_header->length < b_header->length ? a_header->length : b_header->length;
    for(size_t i = 0; i < min; i++) {
        if(a_header->data[i] != b_header->data[i]) {
            return (int) a_header->data[i] - (int) b_header->data[i];
        }
    }
    return a_header->length - b_header->length;
}

bool stringeql(string a, string b) {
    return stringcmp(a, b) == 0;
}

bool stringneql(string a, string b, size_t n) {
    return stringncmp(a, b, n) == 0;
}

size_t stringlen(string input) {
    return getHeaderPointer(input)->length;
}

size_t stringbytesalloced(string input) {
    return getHeaderPointer(input)->allocated_bytes;
}

bool stringeqlidx(string haystack, size_t n, string needle) {
    if(n > stringlen(haystack)) {
        return false;
    }
    size_t remaining_haystack = stringlen(haystack) - n;
    if(remaining_haystack > stringlen(needle)) {
        return false;
    }
    bool needle_matches = false;
    for(size_t index = 0; index < remaining_haystack && index < stringlen(needle); index++) {
        needle_matches =  haystack.at[n + index] == needle.at[index];
        if(!needle_matches) {
            return false;
        }
    }
    return needle_matches;
}

// assume utf-8 is valid
void reverseUtf8_char_old(string to_reverse) {
    for(size_t index = 0; index < stringlen(to_reverse); index++) {
        printf("current character is %08b, char: %c\n", 
            to_reverse.at[index], to_reverse.at[index]);
        if(to_reverse.at[index] < 128) { // normal ascii is not reversed
            continue;       
        }       
        if(((to_reverse.at[index] & 0b1110000) == 0b11000000) && index + 1 < stringlen(to_reverse)) { // 2 byte
            // swap byte with next byte
            printf("detected 2 byte\n");
            unsigned char temp = to_reverse.at[index];
            to_reverse.at[index] = to_reverse.at[index + 1];
            to_reverse.at[index + 1] = temp;
            index += 1; 
            continue;
        }
        if(((to_reverse.at[index] & 0b11110000) == 0b11100000) && index + 2 < stringlen(to_reverse)) { // 3 byte
            // swap byte with next byte
            printf("detected 3 byte\n");
            unsigned char temp = to_reverse.at[index];
            to_reverse.at[index] = to_reverse.at[index + 2];
            to_reverse.at[index + 2] = temp;
            index += 2; 
            continue;
        }
        if(((to_reverse.at[index] & 0b11111000) == 0b11110000) && index + 3 < stringlen(to_reverse)) { // 4 byte
            // swap byte with next byte
            printf("detected 4 byte\n");
            unsigned char temp = to_reverse.at[index];
            to_reverse.at[index] = to_reverse.at[index + 3];
            to_reverse.at[index + 3] = temp;
            temp = to_reverse.at[index + 1];
            to_reverse.at[index + 1] = to_reverse.at[index + 2];
            to_reverse.at[index + 2] = temp;
            index += 3; 
            continue;
        }
    }   
} 


void reverseUtf8_char(char *to_reverse) {
    size_t index = 0;
    while (to_reverse[index] != '\0') {
        printf("current character is %08x, char: %c\n", to_reverse[index], to_reverse[index]);
        if ((unsigned char)to_reverse[index] < 0x80) { // normal ascii is not reversed
            index++;
            continue;
        }
        if ((unsigned char)to_reverse[index] >= 0xc2 && (unsigned char)to_reverse[index] <= 0xdf && to_reverse[index + 1] != '\0') { // 2 byte
            // swap byte with next byte
            printf("detected 2 byte\n");
            char temp = to_reverse[index];
            to_reverse[index] = to_reverse[index + 1];
            to_reverse[index + 1] = temp;
            index += 2;
            continue;
        }
        if ((unsigned char)to_reverse[index] >= 0xe0 && (unsigned char)to_reverse[index] <= 0xef && to_reverse[index + 2] != '\0') { // 3 byte
            // swap byte with next byte
            printf("detected 3 byte\n");
            char temp = to_reverse[index];
            to_reverse[index] = to_reverse[index + 2];
            to_reverse[index + 2] = temp;
            index += 3;
            continue;
        }
        if ((unsigned char)to_reverse[index] >= 0xf0 && (unsigned char)to_reverse[index] <= 0xf4 && to_reverse[index + 3] != '\0') { // 4 byte
            // swap byte with next byte
            printf("detected 4 byte\n");
            char temp = to_reverse[index];
            to_reverse[index] = to_reverse[index + 3];
            to_reverse[index + 3] = temp;
            temp = to_reverse[index + 1];
            to_reverse[index + 1] = to_reverse[index + 2];
            to_reverse[index + 2] = temp;
            index += 4;
            continue;
        }
        index++;
    }
}

// this is utf-8 aware
string stringReverse(string input) {
    reverseUtf8_char_old(input);
    unsigned char temp;
    for(size_t i = 0, evil_i = stringlen(input); i < stringlen(input) / 2; i++, evil_i--) {
        temp = input.at[i];
        input.at[i] = input.at[evil_i - 1];
        input.at[evil_i - 1] = temp;
    }
    // pass over the input and reverse backwards utf8 characters
    return input;
}