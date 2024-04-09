#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "discord.h"
#include "init.h"
#include "logger.h"
#include "middleware.h"
#include "pb_decode.h"
#include "schema.pb.h"
#include "serialize.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (!strncmp(argv[1], "-v", 2)) {
            version();
            return EXIT_SUCCESS;
        }
    }

    new_logger();

    App app;
    struct IDiscordUserEvents user_events;
    struct IDiscordActivityEvents activities_events;
    struct sockaddr_un addr;
    struct sockaddr_un serverinfo;

    int listener = setup(&serverinfo);
    int nvim_reader = conn_nvim(&addr);
    int nvim_writer = accpet_nvim_writer(listener);
    conn_discord(&app, &user_events, &activities_events);

    run(&app, nvim_writer);

    LOG("main", "clean up finished before exit.", INFO);

    close_logger();

    return EXIT_SUCCESS;
}

int accpet_nvim_writer(int listener) {
    struct sockaddr_storage addr;
    int nvim_writer = -1;
    socklen_t addr_size;

    do {
        LOG("serve", "waiting for richpresence.nvim ipc writer to connect", 
                INFO);

        addr_size = sizeof addr;

        if ((nvim_writer = accept(listener, (struct sockaddr *) &addr, 
                        &addr_size)) == -1) {
            LOG("serve", "accept error.", FATAL);
            continue;
        }

        if (addr.ss_family != AF_UNIX) {
            LOG("serve", "accept error - non AF_UNIX connection.", WARN);
            close(nvim_writer);
            nvim_writer = -1;
        }
    } while(nvim_writer < 0);

    LOG("serve", "richpresence.nvim ipc writer is connected", INFO);

    return nvim_writer;
}

void run(App *app, int nvim_writer) {
    char *neovim_state = "Using Neovim";
    char err[128];

    struct timeval tv; tv.tv_sec = 5; tv.tv_usec = 0;
    struct pb_istream_t_state stream_state = { nvim_writer, false, false };
    struct DiscordActivity activity = { 0 }; activity.instance = 0;
    strncpy(activity.state, neovim_state, strlen(neovim_state) + 1);

    enum EDiscordResult result;
    app->activities->update_activity(app->activities, &activity, &result, NULL);

    pb_istream_t stream = pb_istream_from_socket(&stream_state);

    FileChangeEvent event = FileChangeEvent_init_zero;

    setsockopt(nvim_writer, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, 
            sizeof tv);

    do {
        if (!pb_decode_delimited(&stream, FileChangeEvent_fields, &event)) {
            if (!stream_state.closed && !stream_state.istimeout ) {
                sprintf(err, "decode failed: %s", PB_GET_ERROR(&stream));
                LOG("run", "err", WARN);
                continue;
            }

            if (stream_state.closed) break;

            RPC_OK(app->core->run_callbacks(app->core)); continue;
        }

        sprintf(activity.details, "Editing %s", event.filename);
        app->activities->update_activity(app->activities, &activity, &result, 
                NULL);

        RPC_OK(app->core->run_callbacks(app->core));
    } while(1);
}

/*
char state[128] = "Using Neovim";
char details[128] = "Working on richpresence.nvim";

struct DiscordActivity neovim;
memset(&neovim, 0, sizeof neovim);
memcpy(neovim.state, state, sizeof state);
memcpy(neovim.details, details, sizeof details);
neovim.instance = 0;

enum EDiscordResult result; 
app.activities->update_activity(
    app.activities,
    &neovim,
    &result,
    &on_activity_update
);*/
