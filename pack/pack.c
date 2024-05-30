/*
 * The serialization algorithm is a directly referenced and ported from 
 * the implementation of string.pack and string.unpack in lstrlib.c.
 *
 * The following functions do not aim for general purpose use. It builds for the 
 * binary message structure used in Discord RPC. 
 *
 * | opcode | length of JSON data | JSON data |
 * 
 * opcode and length of JSON data must be in 32 bits unsigned integer in LE byte
 *  order.
 */
#include <limits.h>
#include <lua.h>
#include <lua5.1/lua.h>
#include <lua5.1/luaconf.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <stdint.h>


/* number of bits in a character */
#define NB       CHAR_BIT

/* mask for one character (NB 1's) */
#define MC       ((1 << NB) - 1)

#define SZUINT32 32 / CHAR_BIT

static int pack_uint32 (lua_State *L) {
    const lua_Integer is_little = luaL_checkinteger(L, 2);
    luaL_argcheck(L, is_little == 0 || is_little == 1, 2,
            "is_little must be 0 (false, BE) or 1 (true, LE)");

    lua_Integer n = luaL_checkinteger(L, 1);
    luaL_argcheck(L, n < UINT32_MAX, 1, 
            "Providing value exceeds limit of uint32");

    luaL_Buffer lb;
    luaL_buffinit(L, &lb);
    char *b = luaL_prepbuffer(&lb);

    int i;
    b[is_little ? 0 : SZUINT32 - 1] = (char) (n & MC);
    for (i = 1; i < SZUINT32; i++) {
        n >>= NB; b[is_little ? i : SZUINT32 - 1 - i] = (char) (n & MC);
    }
    
    luaL_addsize(&lb, SZUINT32);

    luaL_pushresult(&lb);

    return 1;
}


static int unpack_uint32 (lua_State *L) {
    const lua_Integer is_little = luaL_checkinteger(L, 2);
    luaL_argcheck(L, is_little == 0 || is_little == 1, 2,
            "is_little must be 0 (false, BE) or 1 (true, LE)");

    size_t data_size;
    const char *data = luaL_checklstring(L, 1, &data_size);

    lua_Unsigned n = 0;
    int i;
    const int limit = data_size <= SZUINT32 ? data_size : SZUINT32;
    for (i = limit - 1; i >= 0; i--) {
        if (n > UINT32_MAX) luaL_error(L, "%d-byte cause an uint32 overflow", i);
        n <<= NB;
        n |= (lua_Unsigned) (unsigned char) data[is_little 
            ? i 
            : data_size - 1 - i];
    }
    if (n > UINT32_MAX) luaL_error(L, "The result exceed the maximum of uint32");

    /* check unread bytes, make sure they are zero bits */
    if (data_size > SZUINT32) { 
        for (i = limit; i < data_size; i++) {
            if ((unsigned char)data[is_little ? i : data_size - 1 - i] != 0)
                luaL_error(L, 
                        "%d-byte integer does not fit into Lua Integer",
                        data_size);
        }
    }

    lua_pushinteger(L, (lua_Integer) n);

    return 1;
}


static const luaL_Reg packlib[] = {
    { "pack_uint32", pack_uint32 },
    { "unpack_uint32", unpack_uint32 },
    { NULL, NULL }
};


int luaopen_pack (lua_State *L) {
    luaL_register(L, "pack", packlib);

    return 1;
}
