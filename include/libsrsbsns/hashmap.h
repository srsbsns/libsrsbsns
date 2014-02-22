/* hashmap.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_HASHMAP_H
#define LIBSRSBSNS_HASHMAP_H 1

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*hmap_hash_fn)(const void *elem);
typedef void (*hmap_op_fn)(const void *elem);
typedef void* (*hmap_keydup_fn)(const void *key);
typedef bool (*hmap_eq_fn)(const void *elem1, const void *elem2);
typedef struct hmap *hmap_t;

hmap_t hmap_init(size_t bucketsz, hmap_hash_fn hfn, hmap_eq_fn efn,
    hmap_keydup_fn keydupfn);
void hmap_clear(hmap_t h);
void hmap_dispose(hmap_t h);
void hmap_put(hmap_t h, const void *key, void *elem);
void* hmap_get(hmap_t h, const void *key);
bool hmap_del(hmap_t h, const void *key);
size_t hmap_count(hmap_t h);

bool hmap_first(hmap_t h, void **key, void **val);
bool hmap_next(hmap_t h, void **key, void **val);

void hmap_dump(hmap_t h, hmap_op_fn keyop, hmap_op_fn valop);
void hmap_dumpstat(hmap_t h);

#endif /* LIBSRSBSNS_HASHMAP_H */
