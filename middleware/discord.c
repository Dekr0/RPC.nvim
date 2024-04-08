#include "discord.h"
#include "logger.h"

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
