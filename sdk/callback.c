#include "callback.h"
#include "sdk.h"

void DISCORD_CALLBACK on_user_updated(void *data) {
    Middleware *m = (Middleware *) data;

    struct DiscordUser user; m->users->get_current_user(m->users, &user);

    m->user_id = user.id;
}
