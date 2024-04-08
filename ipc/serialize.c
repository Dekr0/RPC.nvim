#include "pb_encode.h"
#include "serialize.h"

static bool write_callback (pb_ostream_t *stream, const uint8_t *buf, size_t count) {
    int fd = (intptr_t) (stream->state);

    return send(fd, buf, count, 0) == count;
}

pb_ostream_t pb_ostream_from_socket(int fd) {
    pb_ostream_t stream = {&write_callback, (void *)(intptr_t) fd, SIZE_MAX, 0};

    return stream;
}


