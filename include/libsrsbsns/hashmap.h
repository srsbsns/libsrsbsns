/* hashmap.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_HASHMAP_H
#define LIBSRSBSNS_HASHMAP_H 1

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*hmap_hash_fn)(void *elem);
typedef bool (*hmap_eq_fn)(void *elem1, void *elem2);
typedef struct hmap *hmap_t;

hmap_t hmap_init(size_t bucketsz, hmap_hash_fn hfn, hmap_eq_fn efn);
void hmap_dispose(hmap_t h);
void hmap_put(hmap_t h, void *key, void *elem);
void* hmap_get(hmap_t h, void *key);
bool hmap_del(hmap_t h, void *key);

void hmap_dump(hmap_t h);

#endif /* LIBSRSBSNS_HASHMAP_H */
