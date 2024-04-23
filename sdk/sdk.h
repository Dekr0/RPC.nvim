#ifndef RICHPRESENCE_NVIM_H
#define RICHPRESENCE_NVIM_H

#include <lua5.1/lauxlib.h>
#include <sys/un.h>
#include "discord_game_sdk.h"

typedef struct Middleware {
    struct IDiscordCore            *core;
    struct IDiscordActivityManager *activities;
    enum   EDiscordResult          activity_update_result;
} Middleware;

int luaopen_richpresence_sdk (lua_State *);

#endif
