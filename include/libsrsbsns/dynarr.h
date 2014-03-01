/* dynarr.h - (C) 2014, Timo Buhrmester
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_DYNARR_H
#define LIBSRSBSNS_DYNARR_H 1

#include <stdbool.h>
#include <stddef.h>

typedef struct dynarr *dynarr_t;
typedef void (*dynarr_dump_fn)(void *elem);

dynarr_t dynarr_init(size_t initsize);
void dynarr_dispose(dynarr_t d);
void dynarr_clear(dynarr_t d);
void dynarr_truncate(dynarr_t d, size_t fromindex);
size_t dynarr_count(dynarr_t d);
bool dynarr_push(dynarr_t d, void* data);
void* dynarr_get(dynarr_t d, size_t index);
void dynarr_put(dynarr_t d, size_t index, void *elem);
bool dynarr_shrink(dynarr_t d, double countfac);
void dynarr_dump(dynarr_t d, dynarr_dump_fn dfn);

#endif /* LIBSRSBSNS_DYNARR_H */
