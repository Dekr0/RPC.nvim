#include "pb_decode.h"
#include "serialize.h"

static bool read_callback(pb_istream_t *stream, uint8_t *buf, size_t count) {
    int fd = (intptr_t) (stream->state);
    if (count == 0) {
        return true;
    }

    int nbytes = recv(fd, buf, count, MSG_WAITALL);
    if (nbytes == 0) {
        stream->bytes_left = 0;
    }

    return nbytes == count;
}

pb_istream_t pb_istream_from_socket(int fd) {
    pb_istream_t stream = { &read_callback, (void *)(intptr_t) fd, SIZE_MAX };
    
    return stream;
}
