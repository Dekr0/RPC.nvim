#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "sdk.h"

const char *EXT[] = { "c", "lua" };
const char EXT_LEN = 2;

void DISCORD_CALLBACK on_user_updated(void *data) {
    Middleware *m = (Middleware *) data;

    struct DiscordUser user; m->users->get_current_user(m->users, &user);

    m->user_id = user.id;
}

static void conn_neovim(lua_State *L, Middleware *m) {
    luaL_argcheck(L, (m->fd = socket(AF_UNIX, SOCK_STREAM, 0)) != -1, 0, 
            "failed to initialize middleware: socket(): error");

    memset(&(m->addr), 0, sizeof(m->addr));

    m->addr.sun_family = AF_UNIX;
    strncpy(m->addr.sun_path, SOCKET_PATH, sizeof(m->addr.sun_path) - 1);

    luaL_argcheck(L, connect(m->fd, (const struct sockaddr *) &(m->addr), 
                sizeof(m->addr)) != -1, 0, 
            "failed to initialize middleware: connect() error");
}

static void conn_discord(lua_State *L, Middleware *m) {
    static struct IDiscordActivityEvents activities_events;
    static struct IDiscordUserEvents user_events;

    memset(m, 0, sizeof *m);
    memset(&activities_events, 0, sizeof activities_events);
    memset(&user_events, 0, sizeof user_events);

    user_events.on_current_user_update = on_user_updated;

    struct DiscordCreateParams params;
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = m;
    params.activity_events = &activities_events;
    params.user_events = &user_events;

    luaL_argcheck(L,
            DiscordCreate(DISCORD_VERSION, &params, &(m->core)) == DiscordResult_Ok,
            1,
            "failed to connect to Discord"
            );

    m->users = m->core->get_user_manager(m->core);
    m->activities = m->core->get_activity_manager(m->core);
}

static void assign_ext_image (const char *ext, char *large_image) {
    for (size_t i = 0; i < EXT_LEN; i++) {
        if (strcmp(ext, EXT[i]) == 0) {
            strcpy(large_image, ext);
            return;
        }
    }
    strcpy(large_image, "unknown");
}

static void assign_state (const char *filename, const char *ext, char *state) {
    // 1 dot character
    // 16 char for file extension
    // 1 null terminate
    const unsigned long limit = 128 - (strlen("File: ") + 1 + MAX_EXT_LEN + 1);

    strcpy(state, "File: ");

    if (strlen(filename) >= limit) {
        strncat(state, filename, limit - 3);
    } else {
        strcat(state, filename);
    }

    strcat(state, ".");

    if (strlen(ext) >= 16) {
        strncat(state, ext, 16);
    } else {
        strcat(state, ext);
    }
}

static void assign_detail (const char *workspace, const char *mode, char *details) {
    // 16 => longest mode name
    // 1  => for null terminate
    const unsigned long limit =  128 - (strlen("Workspace: ") + 
            strlen("\nMode: ") + 16 + 1);

    if (strlen(workspace) >= limit) {
        strcpy(details, "Workspace: ");
        strncat(details, workspace, limit - 3);
        strcat(details, "...\nMode: ");
        strcat(details, mode);
    } else {
       sprintf(details, "Worksapce: %s\nMode: %s", workspace, mode); 
    }
}

static void activity_init (
        Middleware *m, 
        const char *workspace, 
        const char *filename, 
        const char *ext, // validation done in Lua
        const char *mode
        ) {
    m->neovim_activity.instance = 0;

    if (strlen(filename) == 0 && strlen(ext) == 0) {
        strcpy(m->neovim_activity.assets.large_image, "idle");
    } else {
        strcpy(m->neovim_activity.assets.small_image, "idle");
        assign_ext_image(ext, m->neovim_activity.assets.large_image);
        assign_state(filename, ext, m->neovim_activity.state);
    }
    assign_detail(workspace, mode, m->neovim_activity.details);

    m->activities->update_activity(m->activities, &m->neovim_activity, 
            &m->activity_update_result, NULL);
}

static int init (lua_State *L) {
    Middleware *m = (Middleware *) lua_newuserdata(L, sizeof(Middleware));
    const char *workspace = luaL_checkstring(L, 1);
    const char *filename  = luaL_checkstring(L, 2);
    const char *ext = luaL_checkstring(L, 3);
    const char *mode = luaL_checkstring(L, 4);

    conn_neovim(L, m);
    conn_discord(L, m);
    activity_init(m, workspace, filename, ext, mode);

    return 1;
}

static int run_callback(lua_State *L) {
    Middleware *m = lua_touserdata(L, 1);

    luaL_argcheck(L, m != NULL, 1, "passing nil");

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
