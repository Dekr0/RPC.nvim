#ifndef RICHPRESENCE_NVIM_SERIALIZE
#define RICHPRESENCE_NVIM_SERIALIZE

#include <stdbool.h>
#include <sys/socket.h>
#include "pb.h"

pb_ostream_t pb_ostream_from_socket(int);

#endif
