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

	size_t next;

	heap_cmp_fn cmpfn;
};

heap_t
heap_init(heap_cmp_fn cmpfn)
{
	struct heap *h = malloc(sizeof *h);
	if (!h)
		return NULL;
	
	h->cmpfn = cmpfn;
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

	h->next = h->count = 0;
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

static bool
heap_resize(heap_t h, size_t newsz)
{
	if (!newsz)
		newsz = 1;

	size_t count = heap_count(h);
	if (newsz < count)
		return false;

	void **newloc = realloc(h->tree, newsz * sizeof *newloc);
	if (!newloc)
		return false;
	h->tree = newloc;
	h->treesz = newsz;
	return true;
}

static inline size_t
parent(size_t node)
{
	return (node-1)/2;
}

static inline size_t
left(size_t node)
{
	return 2*node+1;
}

static inline size_t
right(size_t node)
{
	return 2*node+2;
}

static inline void
swap(heap_t h, size_t n1, size_t n2)
{
	void* tmp = h->tree[n1];
	h->tree[n1] = h->tree[n2];
	h->tree[n2] = tmp;
}

static void
upheap(heap_t h, size_t node)
{
	if (!node)
		return;

	size_t par;
	while (h->cmpfn(h->tree[node], h->tree[par = parent(node)]) < 0) {
		swap(h, node, par);
		node = par;
		if (par == 0)
			break;
	}
}

void
static void
downheap(heap_t h, size_t node)
{
	#define M(F,A...) fprintf(stderr, F, ##A)
	M("downheap, initial node %zu!\n", node);
	for (;;) {
		size_t l = left(node);
		size_t r = right(node);
		M("new iteration; node: %zu, left: %zu, right: %zu\n", node, l, r);

		if (l >= h->treesz) {
			M("...would exceed tree storage, done here\n");
			break;
		}

		bool hasleft = h->tree[l];
		bool hasright = h->tree[r];

		bool badleft = hasleft && h->cmpfn(h->tree[l], h->tree[node]) < 0;
		bool badright = hasright && h->cmpfn(h->tree[r], h->tree[node]) < 0;
		M("hasl: %d, hasr: %d, badl: %d, badr: %d\n",
		    hasleft, hasright, badleft, badright);


		size_t n;

		if (badleft && badright) {
			n = h->cmpfn(h->tree[l], h->tree[r]) < 0 ? l : r;
		} else if (badleft || badright) {
			n =  badleft ? l : r;
		} else
			break;

		M("swapping %zu and %zu\n", node, n);
		swap(h, node, n);
		node = n;
	}

	#undef M
}

void
heap_insert(heap_t h, void *elem)
{
	if (!h)
		return;
	
	if (h->next >= h->treesz)
		heap_resize(h, h->treesz*2);
	
	h->tree[h->next] = elem;
	h->count++;
	upheap(h, h->next++);
}

void*
heap_remove(heap_t h)
{
	if (!h || h->count == 0)
		return NULL;
	
	void *res = h->tree[0];
	h->tree[0] = h->tree[--h->next];
	h->count--;
	downheap(h, 0);

	return res;
}

static void
heap_rdump(heap_t h, size_t node, int depth, heap_dump_fn df)
{
	if (node >= h->treesz || node >= h->next || !h->tree[node])
		return;

	heap_rdump(h, left(node), depth+1, df);
	for (int i = 0; i < depth; i++)
		fputs("  ", stderr);
	fputs("``", stderr);
	df(h->tree[node]);
	fprintf(stderr, "'' [%12.12p (p:%12.12p: ``", h->tree[node], h->tree[parent(node)]);
	if (node)
		df(h->tree[parent(node)]);
	else
		fprintf(stderr, "(root)");
	fprintf(stderr, "'')]");
	fputs("\n", stderr);
	heap_rdump(h, right(node), depth+1, df);
}

void
heap_dump(heap_t h, heap_dump_fn df)
{
	fprintf(stderr, "heap %12.12p:\n", h);
	heap_rdump(h, 0, 0, df);
}
