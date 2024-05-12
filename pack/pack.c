#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <stdint.h>

/* number of bits in a character */
#define NB      CHAR_BIT

/* mask for one character (NB 1's) */
#define MC      ((1 << NB) - 1)


static int pack_uint32 (lua_State *L) {
    lua_Integer n = luaL_checkinteger(L, 1);
    const lua_Integer is_little = luaL_checkinteger(L, 2);

    /* TODO: determine whether if overflow occur */

    luaL_argcheck(L, is_little >= 0 && is_little <= 1, 2, 
            "is_little must be 0 (False) or 1 (True)");

    /* TODO: determine size on compile time */

    unsigned char b[4] = { 0 };

    b[is_little ? 0 : 4 - 1] = (char) (n & MC); /* first byte */
    for (size_t i = 1; i < 4; i++) {
        n >>= NB;
        b[is_little ? i : 4 - 1 - i] = (char) (n & MC);
    }

    lua_pushlstring(L, (const char *) &b, 4);

    return 1;
}

static const luaL_Reg packlib[] = {
    { "pack_uint32", pack_uint32 },
    { NULL, NULL }
};

int luaopen_pack (lua_State *L) {
    luaL_register(L, "pack", packlib);

    return 1;
}
