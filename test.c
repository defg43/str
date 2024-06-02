#include "str.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    const char *str = "hello";
    string s = string(str);
    printf("%s\n", s);
    destroyString(s);
    return true;
}

int main() {
    test();
    return 0;
}