#include <errno.h>
#include <fcntl.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NB        128 / CHAR_BIT

/* 
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           random_a                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |          random_a             |  ver  |       random_b        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |var|                       random_c                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           random_c                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * String Representation: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX where X is an 
 * "upper-case" hexadecimal digit [0-9A-F].
 */
static int uuid4 (lua_State *L) {
    int random_data = open("/dev/urandom", O_RDONLY);
    if (random_data < 0) {
        const char * err = strerror(errno);
        luaL_error(L, "Failed to generate UUID 4. Reason: %s", err);
    }

    char uuid4[NB];

    ssize_t result = read(random_data, uuid4, sizeof uuid4);
    if (result < 0) {
        const char *err = strerror(errno);
        luaL_error(L, "Failed to generate UUID 4. Reason: %s", err);
    }

    // Set the 4-7 bits of the 7th byte to 0b0100
    uuid4[6] = (uuid4[6] & 0x0F) | 0x40;  
    // Set the 6-7 bits of the 9th byte to 0b10
    uuid4[8] = (uuid4[8] & 0x3F) | 0x80;

    luaL_Buffer lb;
    luaL_buffinit(L, &lb);
    char *b = luaL_prepbuffer(&lb);

    for (size_t i = 0; i < NB; i++) {
        sprintf(b, "%.2hhX", uuid4[i]);
        b += 2;
        switch (i) {
            case 3:
            case 5: 
            case 7: 
            case 9:
                sprintf(b, "-");
                b++;
                break;
        }
    }

    close(random_data);

    luaL_addsize(&lb, NB * 2 + 4);

    luaL_pushresult(&lb);

    return 1;
} 


static const luaL_Reg uuidlib[] = {
    { "uuid4", uuid4 },
    { NULL, NULL }
};

int luaopen_uuid (lua_State *L) {
    luaL_register(L, "uuid", uuidlib);

    return 1;
}
