#ifndef RICHPRESENCE_NVIM_H
#define RICHPRESENCE_NVIM_H

#include <sys/un.h>
#include "discord_game_sdk.h"

#define CLIENT_ID 1222702873440157796
#define MAX_MODE_LEN 16
#define MAX_EXT_LEN 16
#define SOCKET_PATH "/tmp/nvim.socket"

/*
 * Normal
 * Visual
 * Select
 * Insert
 * Cmdline
 * Ex
 * Terminal
 * Operator-pending
 * Replace
 * Virtual Replace
 * Insert Normal
 * Insert Visual
 * Insert Select
 */

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



#endif
