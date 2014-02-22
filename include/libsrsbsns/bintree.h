/* bintree.h - (C) 2012, Learath2, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_BINTREE_H
#define LIBSRSBSNS_BINTREE_H 1

#include <stdbool.h>

#define TRAV_PREORDER 0
#define TRAV_INORDER 1
#define TRAV_POSTORDER 2

typedef struct bintree *bintree_t;
typedef int (*bintree_cmp_fn)(void *elem1, void *elem2);
typedef void (*bintree_dump_fn)(void *elem);

bintree_t bintree_init(bintree_cmp_fn cmpfn);
void bintree_dispose(bintree_t t);
size_t bintree_count(bintree_t t);
size_t bintree_height(bintree_t t);
void bintree_clear(bintree_t t);
bool bintree_insert(bintree_t t, void *data);
void* bintree_find(bintree_t t, void *data);
bool bintree_remove(bintree_t t, void *data);
void bintree_balance(bintree_t t);

void* bintree_first(bintree_t t, int travtype);
void* bintree_next(bintree_t t);

bool bintree_collect(bintree_t t, void **dest, int travmode);
void bintree_dump(bintree_t t, bintree_dump_fn df);

#endif /* LIBSRSBSNS_BINTREE_H */
