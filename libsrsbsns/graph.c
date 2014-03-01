/* graph.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#include <stdlib.h>

#include <libsrsbsns/deque.h>

#include <libsrsbsns/graph.h>

struct edge {
	struct node *from;
	struct node *to;
	double weight;
	deque_t attr;
};

struct node {
	deque_t ch;
	void *elem;
	deque_t attr;
};


node_t
alloc_node(void* elem)
{
	struct node *n = malloc(sizeof *n);

	n->attr = NULL;
	n->ch = NULL;
	n->elem = elem;

	return n;
}

void*
get_elem(node_t n)
{
	return n->elem;
}

size_t
num_chld(node_t n)
{
	return deque_count(n->ch);
}

size_t
num_nattr(node_t n)
{
	return deque_count(n->attr);
}

edge_t
link_nodes(node_t from, node_t to, double w)
{
	edge_t e = malloc(sizeof *e);

	e->attr = NULL;
	e->from = from;
	e->to = to;
	e->weight = w;

	deque_pushback(from->ch, e);

	return e;
}
