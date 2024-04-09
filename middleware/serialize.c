#include <errno.h>
#include "pb_decode.h"
#include "serialize.h"

static bool read_callback(pb_istream_t *stream, uint8_t *buf, size_t count) {
    struct pb_istream_t_state *state = (struct pb_istream_t_state *) 
        stream->state;
    state->istimeout = 0;
    if (count == 0) {
        return true;
    }

    int nbytes = recv(state->fd, buf, count, MSG_WAITALL);
    if (nbytes == 0) {
        stream->bytes_left = 0;
        state->closed = true;
    }

    if (nbytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            state->istimeout = true;
        }
    }

    return nbytes == count;
}

pb_istream_t pb_istream_from_socket(struct pb_istream_t_state * state) {
    pb_istream_t stream = {
        &read_callback,
        (void *) state,
        SIZE_MAX
    };
    
    return stream;
}
