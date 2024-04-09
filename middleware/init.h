#ifndef RICHPRESENCE_NVIM_INIT_H
#define RICHPRESENCE_NVIM_INIT_H

#include <sys/socket.h>
#include <sys/un.h>
#include "discord.h"

#define NVIM_SOCKET "/tmp/nvim.socket"
#define MIDDLWARE_SOCKET "/tmp/middlware.socket"

int conn_nvim(struct sockaddr_un *);

int setup(struct sockaddr_un *);

void conn_discord(
        App *,
        struct IDiscordUserEvents *,
        struct IDiscordActivityEvents *
        );

#endif
