/* strmap.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_STRMAP_H
#define LIBSRSBSNS_STRMAP_H 1

#include <stddef.h>
#include <stdbool.h>

#include "hashmap.h"

typedef hmap_t smap_t;

smap_t smap_init(size_t bucketsz);
void smap_dispose(smap_t h);
void smap_put(smap_t h, const char *key, const void *elem);
const void* smap_get(smap_t h, const char *key);
bool smap_del(smap_t h, const char *key);

#endif /* LIBSRSBSNS_STRMAP_H */
