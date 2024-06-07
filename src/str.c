#include "../include/str.h"
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
    size_t to_allocate = sizeof(stringHeader_t) + len + 1; 
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

bool strneql(char *a, size_t n, char *b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    if((n > len_a) || (len_b > len_a - n)) {
        return false;
    }
    bool matches = true;
    for(size_t i = 0; i < len_a; i++) {
        matches = (a[i] == b[i]) && matches;
        if(!matches) {
            return false;
        }
    }
    return matches;
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

bool binaryPrefix(unsigned char to_check, unsigned char prefix, size_t depth) {
    byte ones = 0b11111111;
    byte mask = ones << (8 - depth);
    byte result = to_check & mask;
    bool matches = result == prefix;
    #ifdef DEBUG
    printf("to_check: %08b\nprefix:   %08b\nmask:     %08b\nresult:   %08b\nmatches:  %s\n", 
        to_check, prefix, mask, result, matches ? "true" : "false");
    #endif
    return matches;
}

void reverseUtf8_char(char *to_reverse) {
    size_t index = 0;
    while (to_reverse[index] != '\0') {
        // printf("current character is %08x, char: %c\n", to_reverse[index], to_reverse[index]);
        if (binaryPrefix(to_reverse[index], 0b00000000, 1)) { // normal ascii is not reversed
            index++;
            continue;
        }
        if (binaryPrefix(to_reverse[index], 0b11000000, 3) && to_reverse[index + 1] != '\0') {
            // swap byte with next byte
            // printf("detected 2 byte\n");
            char temp = to_reverse[index];
            to_reverse[index] = to_reverse[index + 1];
            to_reverse[index + 1] = temp;
            index += 2;
            continue;
        }
        if (
            binaryPrefix(to_reverse[index], 0b11100000, 4) && to_reverse[index + 2] != '\0') { // 3 byte
            // swap byte with next byte
            //printf("detected 3 byte\n");
            char temp = to_reverse[index];
            to_reverse[index] = to_reverse[index + 2];
            to_reverse[index + 2] = temp;
            index += 3;
            continue;
        }
        if (binaryPrefix(to_reverse[index], 0b11110000, 5) && to_reverse[index + 3] != '\0') { // 4 byte
            // swap byte with next byte
            // printf("detected 4 byte\n");
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
    reverseUtf8_char(input.at);
    unsigned char temp;
    for(size_t i = 0, evil_i = stringlen(input); i < stringlen(input) / 2; i++, evil_i--) {
        temp = input.at[i];
        input.at[i] = input.at[evil_i - 1];
        input.at[evil_i - 1] = temp;
    }
    // pass over the input and reverse backwards utf8 characters
    return input;
}

string sliceFromString(string input, size_t start, size_t end) {
    bool needs_reversing = false;
    if(input.data == NULL) {
        return (string) { .data = NULL };
    }
    if(end > stringlen(input)) {
        if(start > stringlen(input)) {
            return (string) { .data = NULL };
        }
    } else if(start > end) {
        needs_reversing = true;
        size_t temp = start;
        start = end;
        end = temp;
    }
    if(start > stringlen(input)) {
        if(!needs_reversing)   {
            return (string) { .data = NULL };
        } else {
            start = stringlen(input);
        }
    }
    size_t to_allocate = end - start;
    stringHeader_t *buf = malloc(sizeof(stringHeader_t) + to_allocate + 1);
    for(size_t index = 0; index < to_allocate; index++) {
        buf->data[index] = input.at[start + index];
    }
    buf->data[to_allocate] = '\0';
    string ret = (string) { .data = (dataSegmentOfString_t *)buf->data };
    if(needs_reversing) {
        stringReverse(ret);
    }
    return ret;
}

string sliceFromCharPtr(const char *input, size_t start, size_t end) {
    size_t len = strlen(input);
    bool needs_reversing = false;
    if(input == NULL) {
        return (string) { .data = NULL };
    }
    if(end > len) {
        if(start > len) {
            return (string) { .data = NULL };
        }
    } else if(start > end) {
        needs_reversing = true;
        size_t temp = start;
        start = end;
        end = temp;
    }
    if(start > len) {
        if(!needs_reversing)   {
            return (string) { .data = NULL };
        } else {
            start = len;
        }
    }
    size_t to_allocate = end - start;
    stringHeader_t *buf = malloc(sizeof(stringHeader_t) + to_allocate + 1);
    for(size_t index = 0; index < to_allocate; index++) {
        buf->data[index] = input[start + index];
    }
    buf->data[to_allocate] = '\0';
    string ret = (string) { .data = (dataSegmentOfString_t *)buf->data };
    if(needs_reversing) {
        stringReverse(ret);
    }
    return ret;
}

array(string) tokenizeString(string input, string delimiter) {
    string *tokens = NULL;
    for(size_t index = 0; index < stringlen(input); index++) {
        if(stringeqlidx(input, index, delimiter)) {
            stringHeader_t *buf = malloc(sizeof(stringHeader_t) + index + 1);
            for(size_t token_index = 0; token_index < index; token_index++) {
                buf->data[token_index] = input.at[token_index];
            }
            buf->data[index] = '\0';
            string token = (string) { .data = (dataSegmentOfString_t *)buf->data };
            if(tokens == NULL) {
                tokens = malloc(sizeof(string) * 1);
                tokens[0] = token;
            } else {
                array(string) ret = (array(string)) { .element = tokens, .count = 1 };
                ret.element = realloc(ret.element, (ret.count + 1) * sizeof(string));
                ret.element[ret.count++] = token;
                return ret;
            }
            index++;
        }        
    }
    array(string) ret = (array(string)) { .element = tokens, .count = 0 };
    return ret;
}

array(string) tokenizeStringFromCharPtr(string input, char *delimiter) {
	size_t index = 0;
	size_t slice_start = 0;
	size_t len = stringlen(input);
	array(string) ret = { .element = NULL, .count = 0 };
	while(input.at[index]) {
		if(input.at[index] == delimiter[0]) {
			string substr = sliceFromCharPtr(input.at, slice_start, index);
			index++;
			slice_start = index;
			if(ret.count == 0) {
				ret.element = malloc(sizeof(string));
				ret.element[ret.count] = substr;
				ret.count = 1;
			} else {
				ret.element = 
				realloc(ret.element, sizeof(string) * (ret.count + 1));
				ret.element[ret.count] = substr;
				ret.count++;
			} 
		} else {
			index++;
		} 
		// put the remainder into the last token
		if(input.at[index + 1] == '\0' && slice_start != len && index != len) {
			string last_token = sliceFromCharPtr(input.at, slice_start, len);
			if(ret.count == 0) {
				ret.element = malloc(sizeof(string));
				ret.element[ret.count] = last_token;
				ret.count = 1;
			} else {
				ret.element = 
				realloc(ret.element, sizeof(string) * (ret.count + 1));
				ret.element[ret.count] = last_token;
				ret.count++;
			}
		}
	}
	return ret;
}


array(string) tokenizeStringFromCharPtr_old(string input, char *delimiter) { 
    printf("eeeeeeeeeeeeeeeee\n");
    printf("the string to be tokenized is %s", input);
    string *tokens = NULL;
    size_t len = strlen(delimiter);
    for(size_t index = 0; index < stringlen(input); index++) {
        if(strneql((char *)input.data, index, delimiter)) {
            stringHeader_t *buf = malloc(sizeof(stringHeader_t) + index + 1);
            for(size_t token_index = 0; token_index < index; token_index++) {
                buf->data[token_index] = input.at[token_index];
            }
            buf->data[index] = '\0';
            string token = (string) { .data = (dataSegmentOfString_t *)buf->data };
            if(tokens == NULL) {
                tokens = malloc(sizeof(string) * 1);
                tokens[0] = token;
            } else {
                array(string) ret = (array(string)) { .element = tokens, .count = 1 };
                ret.element = realloc(ret.element, (ret.count + 1) * sizeof(string));
                ret.element[ret.count++] = token;
                printf("the count is %ld", ret.count);
                return ret;
            }
            index += len - 1;
        }
    }
    array(string) ret = (array(string)) { .element = tokens, .count = 0 };
    return ret;
}
