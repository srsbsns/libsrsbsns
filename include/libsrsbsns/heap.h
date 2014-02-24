/* heap.h - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_HEAP_H
#define LIBSRSBSNS_HEAP_H 1

#include <stddef.h>
#include <stdbool.h>

typedef struct heap *heap_t;

typedef int (*heap_cmp_fn)(void *elem1, void *elem2);
typedef void (*heap_dump_fn)(void *elem);

heap_t heap_init(heap_cmp_fn cmpfn);

void heap_insert(heap_t h, void *elem);

void heap_clear(heap_t h);
void heap_dispose(heap_t h);
size_t heap_count(heap_t h);
void heap_dump(heap_t h, heap_dump_fn dfn);

void* heap_remove(heap_t h);

#endif /* LIBSRSBSNS_HEAP_H */
