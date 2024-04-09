#ifndef RICHPRESENCE_NVIM_MIDDLEWARE_H
#define RICHPRESENCE_NVIM_MIDDLEWARE_H

#include "discord.h"

int accpet_nvim_writer(int);

void run(App *, int);

void destroy(int, int);

#endif
