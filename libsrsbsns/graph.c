/* graph.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#include <stdlib.h>

#include <libsrsbsns/dynarr.h>

#include <libsrsbsns/graph.h>

struct edge {
	struct node *from;
	struct node *to;
	double weight;
	dynarr_t attr;
};

struct node {
	dynarr_t in;
	dynarr_t out;
	void *elem;
	dynarr_t attr;
};

static void add_edge(dynarr_t *a, edge_t e);
static void del_edge(dynarr_t *a, edge_t e);
static edge_t mkedge(node_t from, node_t to, double w);


node_t
alloc_node(void* elem)
{
	struct node *n = malloc(sizeof *n);

	n->attr = NULL;
	n->in = n->out = NULL;
	n->elem = elem;

	return n;
}

void*
get_elem(node_t n)
{
	return n->elem;
}

void
set_elem(node_t n, void *elem)
{
	n->elem = elem;
}

size_t
fan_out(node_t n)
{
	return !n->out ? 0 : dynarr_count(n->out);
}

size_t
fan_in(node_t n)
{
	return !n->in ? 0 : dynarr_count(n->in);
}

size_t
num_nattr(node_t n)
{
	return !n->attr ? 0 : dynarr_count(n->attr);
}

size_t
num_eattr(edge_t e)
{
	return !e->attr ? 0 : dynarr_count(e->attr);
}

edge_t
link_node(node_t from, node_t to, double w)
{
	edge_t e = mkedge(from, to, w);

	add_edge(&from->out, e);
	add_edge(&to->in, e);

	return e;
}

void
unlink_node(node_t from, node_t to)
{
	drop_edge(find_edge(from, to));
}

edge_t
find_edge(node_t from, node_t to)
{
	void *x;
	if (dynarr_first(from->out, &x))
		do {
			if (((edge_t)x)->to == to)
				return (edge_t)x;
		} while (dynarr_next(from->out, &x));

	return NULL;
}

void
drop_edge(edge_t e)
{
	del_edge(&e->from->out, e);
	del_edge(&e->to->in, e);
}

// -----------------

static void
add_edge(dynarr_t *a, edge_t e)
{
	if (!*a)
		dynarr_put((*a = dynarr_init(1, true)), 0, e);
	else
		dynarr_push(*a, e);
}

static void
del_edge(dynarr_t *a, edge_t e)
{
	ssize_t r = dynarr_findraw(*a, e);
	if (r < 0)
		return;
	
	size_t i = (size_t)r;
	size_t last = dynarr_count(*a) - 1;

	dynarr_put(*a, i, dynarr_get(*a, last));
	dynarr_truncate(*a, last);
}

static edge_t
mkedge(node_t from, node_t to, double w)
{
	edge_t e = malloc(sizeof *e);

	e->attr = NULL;
	e->from = from;
	e->to = to;
	e->weight = w;

	return e;
}
