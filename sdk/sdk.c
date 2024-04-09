#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <string.h>
#include <sys/socket.h>
#include "sdk.h"

void DISCORD_CALLBACK on_user_updated(void *data) {
    Middleware *m = (Middleware *) data;

    struct DiscordUser user;

    m->users->get_current_user(m->users, &user);

    char msg[1024];

    sprintf(msg, "Username: %s; User Id: %ld", user.username, user.id);

    if (send(m->fd, msg, strlen(msg) + 1, 0) == -1) {
        m->senderr = true;
    }
}

static int init (lua_State *L) {
    Middleware *m = (Middleware *) lua_newuserdata(L, sizeof(Middleware));

    luaL_argcheck(L, (m->fd = socket(AF_UNIX, SOCK_STREAM, 0)) != -1, 0, 
            "failed to initialize middleware: socket(): error");

    memset(&(m->addr), 0, sizeof(m->addr));

    m->addr.sun_family = AF_UNIX;
    strncpy(m->addr.sun_path, SOCKET_PATH, sizeof(m->addr.sun_path) - 1);

    luaL_argcheck(L, connect(m->fd, (const struct sockaddr *) &(m->addr), 
                sizeof(m->addr)) != -1, 0, 
            "failed to initialize middleware: connect() error");

    memset(m, 0, sizeof *m);
    memset(m->activities_events, 0, sizeof *(m->activities_events));
    memset(m->user_events, 0, sizeof *(m->user_events));

    m->user_events->on_current_user_update = on_user_updated;

    struct DiscordCreateParams params;
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = m;
    params.activity_events = m->activities_events;
    params.user_events = m->user_events;

    luaL_argcheck(L,
            DiscordCreate(DISCORD_VERSION, &params, &(m->core)) == DiscordResult_Ok,
            1,
            "failed to connect to Discord"
            );

    m->users = m->core->get_user_manager(m->core);
    m->activities = m->core->get_activity_manager(m->core);

    return 1;
}

static int run_callback(lua_State *L) {
    Middleware *m = lua_touserdata(L, 1);

    luaL_argcheck(L, m->core->run_callbacks(m->core) == DiscordResult_Ok, 1,
            "failed to run DiscordSDK callbacks");

    luaL_argcheck(L, !m->senderr, 1, "send error occur");

    return 1;
}

static const struct luaL_Reg sdklib[] = {
    { "init", init },
    { "run_callback", run_callback },
    { NULL, NULL }
};

int luaopen_richpresence_sdk (lua_State *L) {
    luaL_register(L, "sdk", sdklib);

    return 1;
}
