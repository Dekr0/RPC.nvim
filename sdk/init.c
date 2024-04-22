#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "callback.h"
#include "init.h"

const char *EXT[] = { 
    "c",
    "css",
    "git",
    "go",
    "h",
    "js",
    "lua",
    "makefile",
    "md",
    "ts",
    "lua"
};

const char EXT_LEN = 10;

void conn_neovim(lua_State *L, Middleware *m) {
    luaL_argcheck(L, (m->fd = socket(AF_UNIX, SOCK_STREAM, 0)) != -1, 0, 
            "failed to initialize middleware: socket(): error");

    memset(&(m->addr), 0, sizeof(m->addr));

    m->addr.sun_family = AF_UNIX;
    strncpy(m->addr.sun_path, SOCKET_PATH, sizeof(m->addr.sun_path) - 1);

    luaL_argcheck(L, connect(m->fd, (const struct sockaddr *) &(m->addr), 
                sizeof(m->addr)) != -1, 0, 
            "failed to initialize middleware: connect() error");
}

void conn_discord(lua_State *L, Middleware *m) {
    static struct IDiscordActivityEvents activities_events;
    static struct IDiscordUserEvents user_events;

    memset(m, 0, sizeof *m);
    memset(&activities_events, 0, sizeof activities_events);
    memset(&user_events, 0, sizeof user_events);

    user_events.on_current_user_update = on_user_updated;

    static struct DiscordCreateParams params;
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = m;
    params.activity_events = &activities_events;
    params.user_events = &user_events;

    luaL_argcheck(L, DiscordCreate(DISCORD_VERSION, &params, &(m->core)) 
            == DiscordResult_Ok, 1, "failed to connect to Discord");

    luaL_argcheck(L, m->core != NULL, 1, 
            "failed to connect to Discord. IDiscordCore is NULL");

    m->users = m->core->get_user_manager(m->core);

    luaL_argcheck(L, m->users != NULL, 1, 
            "failed to connect to Discord. IDiscordUserManager is NULL");

    m->activities = m->core->get_activity_manager(m->core);

    luaL_argcheck(L, m->activities != NULL, 1, 
            "failed to connect to Discord. IDiscordActivityManager is NULL");
}

void assign_ext_image (const char *ext, char *large_image) {
    for (size_t i = 0; i < EXT_LEN; i++) {
        if (strcasecmp(ext, EXT[i]) == 0) {
            strcpy(large_image, EXT[i]);
            return;
        }
    }
    strcpy(large_image, "unknown");
}

void assign_detail (const char *filename, const char *ext, const char *mode, 
        char *detail) {
    // File: %s.%s | Mode: %s\0
    const unsigned long defined = strlen("Editing ") + 1 + MAX_EXT_LEN + 
        strlen(" | Mode: ") + 1;
    const unsigned long limit = 128 - defined;
    const unsigned long filename_len = strlen(filename);
    const unsigned long ext_len = strlen(ext);

    strcpy(detail, "Editing ");

    if (filename_len >= limit) {
        strncat(detail, filename, limit - 3);
        strcat(detail, "...");
    } else {
        strcat(detail, filename);
    }

    ext_len > 0 ? strcat(detail, ".") : 0;

    ext_len >= 16 ? strncat(detail, ext, 16) : strcat(detail, ext);

    strcat(detail, " | Mode: ");
    strcat(detail, mode);
}

void assign_state (const char *workplace, const unsigned short apm, 
        char *state) {
    // In workplace - %s | APM: %s\0
    const unsigned long defined = strlen("In ") + strlen(" | APM: " ) 
        + MAX_APM_LEN + 1;
    const unsigned long limit =  128 - defined;

    strcpy(state, "In ");

    if (strlen(workplace) >= limit) {
        strncat(state, workplace, limit - 3);
        strcat(state, "...");
    } else {
        strcat(state, workplace);
    }

    strcat(state, " | APM: ");

    char buf[MAX_APM_LEN]; snprintf(buf, MAX_APM_LEN, "%u", apm);

    strcat(state, buf);
}

void update_activity (
        Middleware *m, 
        const char *workplace, 
        const char *filename, 
        const char *ext, // validation done in Lua
        const char *mode,
        const unsigned short apm
        ) {
    if (strlen(filename) == 0 && strlen(ext) == 0) {
        strcpy(m->neovim_activity.assets.large_image, "idle");
    } else {
        strcpy(m->neovim_activity.assets.small_image, "idle");

        assign_ext_image(strlen(ext) == 0 ? filename : ext, 
                m->neovim_activity.assets.large_image);

        assign_detail(filename, ext, mode, m->neovim_activity.details);
    }
    assign_state(workplace, apm, m->neovim_activity.state);

    m->activities->update_activity(m->activities, &m->neovim_activity, 
            &m->activity_update_result, NULL);
}
