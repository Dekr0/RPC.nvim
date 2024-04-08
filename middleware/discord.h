#ifndef RICHPRESENCE_NVIM_DISCORD_H
#define RICHPRESENCE_NVIM_DISCORD_H

#include <assert.h>
#include "discord_game_sdk.h"

#define CLIENT_ID 1222702873440157796

#define RPC_OK(x) assert(x == DiscordResult_Ok)

typedef struct App {
    struct IDiscordCore *core;
    struct IDiscordActivityManager *activities;
    struct IDiscordUserManager *users;
    DiscordUserId user_id;
} App;

void DISCORD_CALLBACK on_activity_update(void *, enum EDiscordResult);

void DISCORD_CALLBACK on_user_updated(void *);

#endif
