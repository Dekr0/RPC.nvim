#ifndef DISCORD_NVIM_MIDDLEWARE_H
#define DISCORD_NVIM_MIDDLEWARE_H

#include <assert.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include "discord_game_sdk.h"
#include "logger.h"

#define CLIENT_ID 1222702873440157796

#define NVIM_SOCKET "/tmp/nvim.socket"
#define MIDDLWARE_SOCKET "/tmp/middlware.socket"

#define RPC_OK(x) assert(x == DiscordResult_Ok)

typedef struct App {
    struct IDiscordCore *core;
    struct IDiscordActivityManager *activities;
    struct IDiscordUserManager *users;
    DiscordUserId user_id;
} App;


typedef void Sigfunc(int);

int neovim(struct sockaddr_un *);

int server(struct sockaddr_un *);

void init(App *, struct IDiscordUserEvents *, struct IDiscordActivityEvents *);

void loop(App *);

void serve(App *, int);

void cancel(int, siginfo_t *, void *);

void destroy(int, int);

void DISCORD_CALLBACK on_activity_update(void *, enum EDiscordResult);

void DISCORD_CALLBACK on_user_updated(void *);

#endif
