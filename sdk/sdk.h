#ifndef RICHPRESENCE_NVIM_H
#define RICHPRESENCE_NVIM_H

#include <sys/un.h>
#include "discord_game_sdk.h"

#define SOCKET_PATH "/tmp/nvim.socket"

#define CLIENT_ID 1222702873440157796

typedef struct Middleware {
    int fd;
    bool senderr; 
    struct sockaddr_un addr;
    struct IDiscordCore *core;
    struct IDiscordActivityEvents *activities_events;
    struct IDiscordActivityManager *activities;
    struct IDiscordUserEvents *user_events;
    struct IDiscordUserManager *users;
} Middleware;

#endif
