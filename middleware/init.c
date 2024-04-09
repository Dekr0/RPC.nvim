#include <unistd.h>
#include "init.h"
#include "logger.h"

int conn_nvim(struct sockaddr_un *addr) {
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        LOG("neovim", "socket error.", FATAL);
    }

    memset(addr, 0, sizeof(*addr));

    addr->sun_family = AF_UNIX;
    strncpy(addr->sun_path, NVIM_SOCKET, sizeof(addr->sun_path) - 1);

    if (connect(fd, (const struct sockaddr *) addr, sizeof(*addr)) == -1) {
        LOG("neovim", "connect error.", FATAL);
    }
    LOG("neovim", "connected to neovim.", INFO);

    return fd;
}

int setup(struct sockaddr_un *serverinfo) {
    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        LOG("server", "socket error.", FATAL);
    }

    memset(serverinfo, 0, sizeof(*serverinfo));

    serverinfo->sun_family = AF_UNIX;
    strncpy(serverinfo->sun_path, MIDDLWARE_SOCKET, 
            sizeof(serverinfo->sun_path) - 1);

    if (bind(fd, (const struct sockaddr *) serverinfo,
                    sizeof(*serverinfo)) == -1) {
        LOG("server", "bind error.", FATAL);
    }

    if (listen(fd, 20) == -1) {
        LOG("server", "listen error.", FATAL);
    }

    return fd;
}

void conn_discord(
        App *app, 
        struct IDiscordUserEvents *user_events,
        struct IDiscordActivityEvents *activities_events
) {

    memset(app, 0, sizeof *app);
    memset(user_events, 0, sizeof *user_events);
    memset(activities_events, 0, sizeof *activities_events);

    user_events->on_current_user_update = on_user_updated;

    struct DiscordCreateParams params;
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = app;
    params.activity_events = activities_events;
    params.user_events = user_events;

    RPC_OK(DiscordCreate(DISCORD_VERSION, &params, &(app->core)));

    app->users = app->core->get_user_manager(app->core);
    app->activities = app->core->get_activity_manager(app->core);
}

void destroy(int nvim, int server) {
    close(nvim);
    close(server);
}
