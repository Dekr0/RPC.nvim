#ifndef RICHPRESENCE_NVIM_SERIALIZE_H
#define RICHPRESENCE_NVIM_SERIALIZE_H

#include <sys/socket.h>
#include "pb.h"

struct pb_istream_t_state {
    int fd;
    bool istimeout;
    bool closed;
};

pb_istream_t pb_istream_from_socket(struct pb_istream_t_state *);

#endif
