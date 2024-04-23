#ifndef RICHPRESENCE_NVIM_INIT_H
#define RICHPRESENCE_NVIM_INIT_H

#include <lua5.1/lauxlib.h>
#include "sdk.h"

#define CLIENT_ID 1222702873440157796
#define MAX_MODE_LEN 16
#define MAX_EXT_LEN 16
#define MAX_APM_LEN 3
#define SOCKET_PATH "/tmp/nvim.socket"

void conn_discord(lua_State *L, Middleware *);

void assign_ext_image(const char *, char *);

void assign_detail(const char *, const char *, const char *, char *);

void assign_state(const char *, const unsigned short, char *);

void update_activity(
        Middleware *, 
        const char *, 
        const char *, 
        const char *,
        const char *,
        const unsigned short
    );

#endif
