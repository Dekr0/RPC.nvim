#include "middleware.h"

void loop(App *app) {
    for (; ;) {
        // https://discord.com/developers/docs/game-sdk/discord#runcallbacks
        RPC_OK(app->core->run_callbacks(app->core));
#ifdef _WIN32
        Sleep(16);
#else
        LOG("loop", "sleep for 5 seconds.", INFO);
        usleep(5000000);
#endif
    }
}

void handle(App *app, int neovim) {
    int nbytes;
    char buffer[64];

    while ((nbytes = recv(neovim, buffer, sizeof buffer, 0)) > 0) {
        buffer[sizeof(buffer) - 1] = 0;
        LOG("serve", buffer, INFO);
        memset(buffer, 0, sizeof buffer);
    }

    if (nbytes == -1) {
        LOG("serve", "close richpresence.nvim ipc writer connection due to recv error",
                WARN);
    } else {
        LOG("serve", "richpresence.nvim ipc writer close connection gracefully", 
                WARN);
    }
}

void serve(App *app, int server) {
    struct sockaddr_storage addr;
    int neovim = -1;
    socklen_t addr_size;

    do {
        LOG("serve", "waiting for richpresence.nvim ipc writer to connect", 
                INFO);

        addr_size = sizeof addr;

        if ((neovim = accept(server, (struct sockaddr *) &addr, &addr_size)) 
                == -1) {
            LOG("serve", "accept error.", FATAL);
            continue;
        }

        if (addr.ss_family != AF_UNIX) {
            LOG("serve", "accept error - non AF_UNIX connection.", WARN);
            close(neovim);
            neovim = -1;
        }
    } while(neovim < 0);

    LOG("serve", "richpresence.nvim ipc writer is connected", INFO);

    handle(app, neovim);

    close(neovim);
}

void cancel(int signo, siginfo_t *info, void *context) {

}

void DISCORD_CALLBACK on_activity_update(
        void *callback_data, 
        enum EDiscordResult result
) {
    LOG("on_activity_update", "called.", INFO);
    RPC_OK(result);
}

void DISCORD_CALLBACK on_user_updated(void *data) {
    struct App *app = (struct App *) data;
    struct DiscordUser user;
    LOG("on_user_updated", "called.", INFO);
    app->users->get_current_user(app->users, &user);
    app->user_id = user.id;
}



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

    int nvim = neovim(&addr);
    int s = server(&serverinfo);
    init(&app, &user_events, &activities_events);

    // loop(&app);
    
    serve(&app, s);

    destroy(nvim, s);

    LOG("main", "clean up finished before exit.", INFO);

    close_logger();

    return EXIT_SUCCESS;
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
