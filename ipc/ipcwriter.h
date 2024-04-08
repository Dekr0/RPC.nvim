#ifndef DISCORD_NVIM_LUA_FLAT_H 
#define DISCORD_NVIM_LUA_FLAT_H

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MIDDLWARE_SOCKET "/tmp/middlware.socket" // neovim server write to middleware
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(RichPresenceNvim, x) // Specified in the schema.

typedef struct IPCWriter {
    int fd;
    struct sockaddr_un addr;
} IPCWriter;

static int new (lua_State *);

static int destroy (lua_State *);

static int filechange (lua_State *);

static int marshal_test (lua_State *);

static int ping (lua_State *);

int luaopen_ipcwriter (lua_State *);

#endif
