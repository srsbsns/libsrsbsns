/* hashmap.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_HASHMAP_H
#define LIBSRSBSNS_HASHMAP_H 1

#include <stddef.h>
#include <stdbool.h>

typedef size_t (*hmap_hash_fn)(void *elem);
typedef size_t (*hmap_eq_fn)(void *elem1, void *elem2);
typedef struct hmap *hmap_t;

#endif /* LIBSRSBSNS_HASHMAP_H */
