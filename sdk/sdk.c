#include <string.h>
#include <unistd.h>
#include "sdk.h"
#include "init.h"

static int init (lua_State *L) {
    Middleware *m = (Middleware *) lua_newuserdata(L, sizeof(Middleware));
    const char *workspace = luaL_checkstring(L, 1);
    const char *filename  = luaL_checkstring(L, 2);
    const char *ext = luaL_checkstring(L, 3);
    const char *mode = luaL_checkstring(L, 4);
    const unsigned apm = luaL_checknumber(L, 5);

    conn_neovim(L, m);
    conn_discord(L, m);
    activity_init(m, workspace, filename, ext, mode, apm);

    return 1;
}

static int run_callback(lua_State *L) {
    Middleware *m = lua_touserdata(L, 1);
    const char *workplace = luaL_checkstring(L, 2);
    const char *filename  = luaL_checkstring(L, 3);
    const char *ext = luaL_checkstring(L, 4);
    const char *mode = luaL_checkstring(L, 5);
    const unsigned apm = luaL_checknumber(L, 6);

    luaL_argcheck(L, m != NULL, 1, "passing nil");

    if (strlen(filename) == 0 && strlen(ext) == 0) {
        strcpy(m->neovim_activity.assets.large_image, "idle");
    } else {
        strcpy(m->neovim_activity.assets.small_image, "idle");
        assign_ext_image(ext, m->neovim_activity.assets.large_image);
        assign_detail(filename, ext, mode, m->neovim_activity.details);
    }
    assign_state(workplace, apm, m->neovim_activity.state);

    m->activities->update_activity(m->activities, &m->neovim_activity, 
            &m->activity_update_result, NULL);

    luaL_argcheck(L, m->core->run_callbacks(m->core) == DiscordResult_Ok, 1,
            "failed to run DiscordSDK callbacks");

    luaL_argcheck(L, !m->senderr, 1, "send error occur");

    lua_pushinteger(L, m->user_id);

    return 1;
}

static int clean(lua_State *L) {
    Middleware *m = lua_touserdata(L, 1);

    luaL_argcheck(L, m != NULL, 1, "passing nil");

    close(m->fd);

    m->activities->clear_activity(m->activities, &m->activity_update_result, 
            NULL);

    luaL_argcheck(L, m->core->run_callbacks(m->core) == DiscordResult_Ok, 1,
            "failed to run DiscordSDK callbacks");

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
