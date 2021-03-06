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

#define PARENT(NODE) (((NODE)-1)/2)
#define LEFT(NODE) (2*(NODE)+1)
#define RIGHT(NODE) (2*(NODE)+2)

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
	h->next = h->count = 0;
}

void
heap_dispose(heap_t h)
{
	free(h->tree);
	free(h);
}

size_t
heap_count(heap_t h)
{
	return h->count;
}

static bool
heap_resize(heap_t h, size_t newsz)
{
	if (!newsz)
		newsz = 1;

	if (newsz < heap_count(h))
		return false;

	void **newloc = realloc(h->tree, newsz * sizeof *newloc);
	if (!newloc)
		return false;

	h->tree = newloc;
	h->treesz = newsz;
	return true;
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
	while (h->cmpfn(h->tree[node], h->tree[par = PARENT(node)]) < 0) {
		swap(h, node, par);
		node = par;
		if (par == 0)
			break;
	}
}

static void
downheap(heap_t h)
{
	size_t l, r, n;
	size_t node = 0;
	while ((r = RIGHT(node)), ((l = LEFT(node)) < h->next)) {
		bool badleft = h->cmpfn(h->tree[l], h->tree[node]) < 0;
		bool badright = r < h->next
		    && h->cmpfn(h->tree[r], h->tree[node]) < 0;

		if (badleft && badright)
			n = h->cmpfn(h->tree[l], h->tree[r]) < 0 ? l : r;
		else if (badleft || badright)
			n =  badleft ? l : r;
		else
			break;

		swap(h, node, n);
		node = n;
	}
}

void
heap_insert(heap_t h, void *elem)
{
	if (!elem)
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
	if (h->count == 0)
		return NULL;
	
	void *res = h->tree[0];
	h->tree[0] = h->tree[--h->next];
	h->count--;
	downheap(h);

	return res;
}

static void
heap_rdump(heap_t h, size_t node, int depth, heap_dump_fn df)
{
	if (node >= h->treesz || node >= h->next || !h->tree[node])
		return;

	heap_rdump(h, LEFT(node), depth+1, df);
	for (int i = 0; i < depth; i++)
		fputs("  ", stderr);
	fputs("``", stderr);
	df(h->tree[node]);
	fprintf(stderr, "'' [%zu; %12p (p:%12p: ``",
	    node, h->tree[node], h->tree[PARENT(node)]);
	if (node)
		df(h->tree[PARENT(node)]);
	else
		fprintf(stderr, "(root)");
	fprintf(stderr, "'')]");
	fputs("\n", stderr);
	heap_rdump(h, RIGHT(node), depth+1, df);
}

void
heap_dump(heap_t h, heap_dump_fn df)
{
	fprintf(stderr, "heap %12p:\n", h);
	heap_rdump(h, 0, 0, df);
}
