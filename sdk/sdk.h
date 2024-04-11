#ifndef RICHPRESENCE_NVIM_H
#define RICHPRESENCE_NVIM_H

#include <lua5.1/lauxlib.h>
#include <sys/un.h>
#include "discord_game_sdk.h"

typedef struct Middleware {
    int fd;
    bool senderr; 
    struct sockaddr_un addr;
    struct IDiscordCore *core;
    struct IDiscordActivityManager *activities;
    struct IDiscordUserManager *users;
    struct DiscordActivity neovim_activity;
    enum EDiscordResult activity_update_result;
    DiscordUserId user_id;
} Middleware;

int luaopen_richpresence_sdk (lua_State *);

#endif
