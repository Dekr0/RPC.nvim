#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sdk.h"
#include "state.h"

static int init (lua_State *L) {
    Middleware *m = (Middleware *) lua_newuserdata(L, sizeof(Middleware));

    luaL_argcheck(L, m != NULL, 1, "failed to allocate memory for middleware inside Lua stack");

    const char *workspace = luaL_checkstring(L, 1);
    const char *filename  = luaL_checkstring(L, 2);
    const char *ext       = luaL_checkstring(L, 3);
    const char *mode      = luaL_checkstring(L, 4);
    const unsigned apm    = luaL_checknumber(L, 5);

    conn_discord(L, m);

    update_activity(m, workspace, filename, ext, mode, apm);

    return 1;
}

static int run_callback(lua_State *L) {
    Middleware *m         = lua_touserdata(L, 1);
    const char *workplace = luaL_checkstring(L, 2);
    const char *filename  = luaL_checkstring(L, 3);
    const char *ext       = luaL_checkstring(L, 4);
    const char *mode      = luaL_checkstring(L, 5);
    const unsigned apm    = luaL_checknumber(L, 6);

    luaL_argcheck(L, m != NULL, 1, "passing nil");

    update_activity(m, workplace, filename, ext, mode, apm);

    luaL_argcheck(L, m->core != NULL, 1, "failed to run DiscordSDK callbacks. IDiscordcore is NULL");

    luaL_argcheck(L, m->core->run_callbacks(m->core) == DiscordResult_Ok, 1, "failed to run DiscordSDK callbacks");

    return 0;
}

static int clean(lua_State *L) {
    Middleware *m = lua_touserdata(L, 1);

    luaL_argcheck(L, m != NULL, 1, "passing nil");

    m->activities->clear_activity(m->activities, &m->activity_update_result, NULL);

    luaL_argcheck(L, m->core->run_callbacks(m->core) == DiscordResult_Ok, 1, "failed to run DiscordSDK callbacks");

    return 0;
}

static const struct luaL_Reg sdklib[] = {
    { "init", init },
    { "clean", clean },
    { "run_callback", run_callback },
    { NULL, NULL }
};

int luaopen_richpresence_sdk (lua_State *L) {
    luaL_register(L, "sdk", sdklib);

    return 1;
}
