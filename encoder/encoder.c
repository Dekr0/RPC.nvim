#include <string.h>

#include "encoder.h"

static int is_little_endian() {
    unsigned int v = 0x1;
    char *p = (char *) &v;
    return *p == 1 ? 1 : 0;
}

static int to_little_endian(int v) {
    int r = 0;

    r |= ((0xff & v) << 24);
    r |= (((0xff << 8) & v) <<8);
    r |= (((0xff << 16) & v) >> 8);
    r |= (((0xff << 24) & v) >> 24);

    return r;
}

static int to_big_endian(int v) {
    int r = 0;

    r |= ((0x000000ff & v) << 24);
    r |= ((0x0000ff00 & v) << 8);
    r |= ((0x00ff0000 & v) >> 8);
    r |= ((0xff000000 & v) >> 24);

    return r;
}

static void packi32(unsigned char *buf, unsigned long int i) {
    *buf++ = i >> 24;
    *buf++ = i >> 16;
    *buf++ = i >> 8;
    *buf++ = i;
}

static long int unpacki32(unsigned char *buf) {
    unsigned long int i2 = ((unsigned long int) buf[0] << 24) |
                           ((unsigned long int) buf[1] << 16) |
                           ((unsigned long int) buf[2] << 8)  |
                           buf[3];
    long int i;

    if (i2 <= 0x7fffffffu)
        i = i2;
    else
        i = -1 - (long int)(0xffffffffu - i2);

    return i;
}

static int write_i32_little_endian(lua_State *L) {
    int v = luaL_checknumber(L, 1);

    !is_little_endian() && (v = to_little_endian(v));

    unsigned char buf[4]; memset(buf, 0, 4); packi32(buf, v);

    lua_pushlstring(L, (const char *) buf, 4);

    return 1;
}

static const struct luaL_Reg encoder[] = {
    { "write_i32_little_endian", write_i32_little_endian },
    { NULL, NULL }
};

int luaopen_RPC_encoder (lua_State *L) {
    luaL_register(L, "encoder", encoder);

    return 1;
}
