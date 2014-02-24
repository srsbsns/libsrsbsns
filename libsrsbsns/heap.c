/* heap.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libsrsbsns/heap.h>

struct heap {
	void **tree;
	size_t treesz;
	size_t count;

	heap_cmp_fn cmpfn;
};

heap_t
heap_init(heap_cmp_fn cmpfn)
{
	struct heap *h = malloc(sizeof *h);
	if (!h)
		return NULL;
	
	h->treesz = 32;
	h->tree = malloc(h->treesz * sizeof *h->tree);

	heap_clear(h);

	return h;
}

void
heap_clear(heap_t h)
{
	if (!h)
		return;

	for(size_t i = 0; i < h->treesz; i++)
		h->tree[i] = NULL;

	h->count = 0;
}

void
heap_dispose(heap_t h)
{
	if (!h)
		return;

	free(h->tree);
}

size_t
heap_count(heap_t h)
{
	return !h ? 0 : h->count;
}

void
heap_dump(heap_t h, heap_dump_fn dfn)
{
	#define M(X, A...) fprintf(stderr, X, ##A)
	M("===heap dump (count: %zu)===\n", h->count);
	if (!h)
		M("nullpointer...\n");

	//...

	M("===end of heap dump===\n");
	#undef M
}
