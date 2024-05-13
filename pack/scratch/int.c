#include <limits.h>
#include <stdio.h>
#include <lua.h>

int main(void) {
    lua_Integer a = LLONG_MAX + 1;
    printf("%d", a >= 0);
}
