#include "ipcwriter.h"
#include "pb_encode.h"
#include "schema.pb.h"
#include "serialize.h"
#include <stdbool.h>

static int new (lua_State *L) {
    IPCWriter *w = (IPCWriter *) lua_newuserdata(L, sizeof(IPCWriter));

    luaL_argcheck(L, (w->fd = socket(AF_UNIX, SOCK_STREAM, 0)) != -1, 0, 
            "failed to initialize IPC writer: socket() error");

    memset(&(w->addr), 0, sizeof(w->addr));
    w->addr.sun_family = AF_UNIX;
    strncpy(
        w->addr.sun_path,
        MIDDLWARE_SOCKET,
        sizeof(w->addr.sun_path) - 1
    );

    luaL_argcheck(L, (connect(w->fd, (const struct sockaddr *) &(w->addr), 
                    sizeof(w->addr))) != -1, 0, 
            "failed to initialize IPC writer: connect() error");

    return 1;
}

static int destroy (lua_State *L) {
    IPCWriter *w = (IPCWriter *) lua_touserdata(L, 1);

    luaL_argcheck(L, w != NULL, 1, "'IPCWriter' expected");

    close(w->fd);

    return 1;
}

static int filechange (lua_State *L) {
    IPCWriter *w = (IPCWriter *) lua_touserdata(L, 1);
    const char *f = luaL_checkstring(L, 2);

    luaL_argcheck(L, w != NULL, 1, "'IPCWriter' expected");

    if (strlen(f) == 0) {
        return 1;
    }

    // Shortend the filename if filename is longer to 258
    // Retain parts of the filename and file type

    FileChangeEvent event = FileChangeEvent_init_zero;
    strncpy(event.filename, f, strlen(f) + 1);

    pb_ostream_t stream = pb_ostream_from_socket(w->fd);

    luaL_argcheck(L, pb_encode_delimited(&stream, FileChangeEvent_fields, 
                &event), 1, stream.errmsg);

    return 1;
}

static int marshal_test (lua_State *L) {
    int value = luaL_checkint(L, 1);

    const size_t size = 10;

    Test msg = {value};

    uint8_t buffer[size];
    char blob[size];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    luaL_argcheck(L, pb_encode(&stream, Test_fields, &msg), 1,
            "marshal test failed: failed to encode");

    for (size_t i = 0; i < size; i++) {
        blob[i] = buffer[i];
    }

    lua_pushstring(L, blob);

    return 1;
}

static int ping (lua_State *L) {
    IPCWriter *writer = (IPCWriter *) lua_touserdata(L, 1);
    int counter = luaL_checkint(L, 2);

    luaL_argcheck(L, writer != NULL, 1, "'IPCWriter' is expected.");

    int nbytes;
    char buffer[16];

    sprintf(buffer, "Ping %d\n", counter);
    if ((nbytes = send(writer->fd, buffer, strlen(buffer) + 1, 0)) == -1) {
        luaL_argcheck(L, nbytes != -1, 1, "IPC write error.");
        return 0;
    }

    return 1;
}

static const struct luaL_Reg ipcwriterlib [] = {
    { "new", new },
    { "destroy", destroy },
    { "filechange", filechange },
    { "marshal_test", marshal_test },
    { "ping", ping },
    { NULL, NULL }
};

int luaopen_richpresence_ipcwriter (lua_State *L) {
    luaL_register(L, "ipcwriter", ipcwriterlib);
    return 1;
}
