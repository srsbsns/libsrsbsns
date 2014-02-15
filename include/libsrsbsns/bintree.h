#ifndef LIBSRSBSNS_BINTREE_H
#define LIBSRSBSNS_BINTREE_H 1

#include <stdbool.h>

typedef struct bintree *bintree_t;
typedef int (*bintree_cmp_fn)(void *elem1, void *elem2);

bintree_t bintree_init(bintree_cmp_fn cmpfn);
void bintree_dispose(bintree_t t);
int bintree_count(bintree_t t);
void bintree_clear(bintree_t t);
static void bintree_rclear(bt_node *n);
bool bintree_insert(bintree_t t, void *data);
static bt_node* bintree_nodefind(bintree_t t, void *data);
void* bintree_find(bintree_t t, void *data);
bool bintree_remove(bintree_t t, void *data);
void bintree_swapnode(bt_node *a, bt_node *b);

#endif /* LIBSRSBSNS_BINTREE_H */