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

static void
heap_rdump(struct heap_node *n, int depth, heap_dump_fn dfn)
{
	if (!n)
		return;

	heap_rdump(n->left, depth+1, df);
	for (int i = 0; i < depth; i++)
		fputs("  ", stderr);
	fputs("``", stderr);
	df(n->data);
	fprintf(stderr, "'' [%12.12p (p:%12.12p: ``", n, n->parent);
	if (n->parent)
		df(n->parent->data);
	else
		fprintf(stderr, "(root)");
	fprintf(stderr, "'')]");
	fputs("\n", stderr);
	heap_rdump(n->right, depth+1, df);
}

void
heap_dump(heap_t t, heap_dump_fn dfn)
{
	fprintf(stderr, "heap %12.12p:\n", t);
	heap_rdump(t->root, 0, df);
}
