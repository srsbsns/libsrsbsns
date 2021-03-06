/* strmap.h - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_STRMAP_H
#define LIBSRSBSNS_STRMAP_H 1

#include <stddef.h>
#include <stdbool.h>

#include <sys/types.h>

#include "hashmap.h"

typedef hmap_t smap_t;

smap_t smap_init(size_t bucketsz);
void smap_clear(smap_t h);
void smap_dispose(smap_t h);
void smap_put(smap_t h, const char *key, void *elem);
void* smap_get(smap_t h, const char *key);
bool smap_del(smap_t h, const char *key);
size_t smap_count(smap_t h);

bool smap_first(smap_t h, const char **key, void **val);
bool smap_next(smap_t h, const char **key, void **val);

#endif /* LIBSRSBSNS_STRMAP_H */
