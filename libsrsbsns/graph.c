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
