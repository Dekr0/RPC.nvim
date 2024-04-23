#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include "state.h"
#include "discord_game_sdk.h"

const char EXT_LEN = 10;

// File: %s.%s | Mode: %s\0
const unsigned long DETAIL_LIMIT = 128 - (strlen("Editing ") + 1 + MAX_EXT_LEN + strlen(" | Mode: ") + 1);

// In workplace - %s | APM: %s\0
const unsigned long STATE_LIMIT  = 128 - (strlen("In ") + strlen(" | APM: " ) + MAX_APM_LEN + 1);

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

void conn_discord(lua_State *L, Middleware *m) {
    memset(m, 0, sizeof(*m));

    struct DiscordCreateParams params;

    DiscordCreateParamsSetDefault(&params);

    params.client_id = CLIENT_ID;
    params.flags     = DiscordCreateFlags_Default;

    luaL_argcheck(L, DiscordCreate(DISCORD_VERSION, &params, &(m->core))
            == DiscordResult_Ok, 1, "failed to connect to Discord");

    luaL_argcheck(L, m->core != NULL, 1,
            "failed to connect to Discord. IDiscordCore is NULL");

    m->activities = m->core->get_activity_manager(m->core);

    luaL_argcheck(L, m->activities != NULL, 1, 
            "failed to connect to Discord. IDiscordActivityManager is NULL");
}

void assign_ext_image(const char *ext, char *large_image) {
    for (size_t i = 0; i < EXT_LEN; i++) {
        if (strcasecmp(ext, EXT[i]) == 0) {
            strcpy(large_image, EXT[i]);
            return;
        }
    }
    strcpy(large_image, "unknown");
}

void assign_detail(const char *filename, const char *ext, const char *mode, 
        char *detail) {
    const unsigned long filename_len = strlen(filename);
    const unsigned long ext_len = strlen(ext);

    strcpy(detail, "Editing ");

    if (filename_len >= DETAIL_LIMIT) {
        strncat(detail, filename, DETAIL_LIMIT - 3);
        strcat(detail, "...");
    } else {
        strcat(detail, filename);
    }

    ext_len > 0 ? strcat(detail, ".") : 0;

    ext_len >= 16 ? strncat(detail, ext, 16) : strcat(detail, ext);

    strcat(detail, " | Mode: ");
    strcat(detail, mode);
}

void assign_state(const char *workplace, const unsigned short apm, 
        char *state) {
    strcpy(state, "In ");

    if (strlen(workplace) >= STATE_LIMIT) {
        strncat(state, workplace, STATE_LIMIT - 3);
        strcat(state, "...");
    } else {
        strcat(state, workplace);
    }

    strcat(state, " | APM: ");

    char buf[MAX_APM_LEN]; snprintf(buf, MAX_APM_LEN, "%u", apm);

    strcat(state, buf);
}

void update_activity(
        Middleware *m, 
        const char *workplace, 
        const char *filename, 
        const char *ext, // validation done in Lua
        const char *mode,
        const unsigned short apm
        ) {

    static struct DiscordActivity neovim = {0};

    if (!neovim.timestamps.start) {
        neovim.timestamps.start = time(0);
    }

    memset(neovim.assets.small_image, 0, 128);
    memset(neovim.assets.large_image, 0, 128);
    memset(neovim.details,            0, 128);
    memset(neovim.state,              0, 128);

    if (strlen(filename) == 0 && strlen(ext) == 0) {
        strcpy(neovim.assets.large_image, "idle");
    } else {
        strcpy(neovim.assets.small_image, "idle");

        assign_ext_image(strlen(ext) == 0 ? filename : ext, neovim.assets.large_image);

        assign_detail(filename, ext, mode, neovim.details);
    }
    assign_state(workplace, apm, neovim.state);

    m->activities->update_activity(m->activities, &neovim, &m->activity_update_result, NULL);
}
