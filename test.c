#include "include/str.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

bool test() {
    const char *str = "hello";
    string s = string(str);
    printf("%s\n", s);
    printf("the character at index 0 is %c\n", s.at[0]);
    s.at[0] = 'b';
    printf("the string is %s\n", s);
    destroyString(s);
    static_assert(!__builtin_types_compatible_p(char *, string), "they are compatible");

    string str1 = string("hello");
    string str2 = string("world");
    string str3 = concat(str1, str2);
    printf("%s\n", str3);
    destroyString(str1);
    destroyString(str2);
    destroyString(str3);

    string str4 = string("helloaa");
    string str5 = string("hello");
    int result = stringcmp(str4, str5);
    printf("%d\n", result);
    result = stringncmp(str4, str5, 4);
    printf("%d\n", result);
    printf("str4 and str5 are %s\n", stringeql(str4, str5) ? "equal" : "not equal");
    string haystack = string("hello world");
    string needle = string("world");
    printf("needle in haystack: %s\n", stringeqlidx(haystack, 6, needle) ? "true" : "false");
    destroyString(str4);
    destroyString(str5);
    destroyString(haystack);
    destroyString(needle);
    
    string str6 = string("世界!🌍😊éöñçїɔ㊙️⚛️🏳️‍🌈.");
    string str7 = string(str6);
    assert(stringeql(str7, stringReverse(stringReverse(str6))));
    printf("%s\n", str6);
    printf("%s\n", stringReverse(str6));
    destroyString(str6);
    destroyString(str7);
    string str8 = string("abc test 123");
    string str9 = sliceFromString(str8, 4, 7);
    printf("%s\n", str9);
    destroyString(str8);
    destroyString(str9);
    string str10 = sliceFromCharPtr("abc test 123", 4, 255);
    printf("%s\n", str10);
    destroyString(str10);
    return true;
}

bool test2() {
    array(string) tokens = tokenize("hello world, this is a test", " ");
    printf("the count is %ld\n", tokens.count);
    for(size_t i = 0; i < tokens.count; i++) {
        printf("el. %ld : %s\n", tokens.count, tokens.element[i]);
    }
    printf("tokenization done\n");
    for(size_t i = 0; i < tokens.count; i++) {
        destroyString(tokens.element[i]);
    }
    free(tokens.element);
    return true;
}

int main() {
    test2();
    return 0;
}
