/* ringbuf.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_RINGBUF_H
#define LIBSRSBSNS_RINGBUF_H 1

#include <stddef.h>
#include <stdbool.h>

typedef struct ringbuf *ringbuf_t;

ringbuf_t ringbuf_init(size_t initsize);
void ringbuf_dispose(ringbuf_t b);
bool ringbuf_empty(ringbuf_t b);
void ringbuf_clear(ringbuf_t b);
size_t ringbuf_count(ringbuf_t b);
size_t ringbuf_get(ringbuf_t b, unsigned char *data, size_t num);
size_t ringbuf_peek(ringbuf_t b, unsigned char *data, size_t num);
void ringbuf_put(ringbuf_t b, unsigned char *data, size_t num);
void ringbuf_dump(ringbuf_t b);

#endif /* LIBSRSBSNS_RINGBUF_H */
