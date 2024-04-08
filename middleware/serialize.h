#ifndef RICHPRESENCE_NVIM_SERIALIZE_H
#define RICHPRESENCE_NVIM_SERIALIZE_H

#include <stdbool.h>
#include <sys/socket.h>
#include "pb.h"

pb_istream_t pb_istream_from_socket(int);

#endif
