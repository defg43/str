#include "str.h"
#include <stdio.h>
#include <stdbool.h>

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
    
    string str6 = string("helloß");
    printf("before reversing: %s\n", str6);
    printf("->%s<-\n", stringReverse(str6));
    destroyString(str6);
    
    return true;
}

int main() {
    test();
    return 0;
}