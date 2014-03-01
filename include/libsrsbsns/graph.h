/* graph.h - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_GRAPH_H
#define LIBSRSBSNS_GRAPH_H 1

typedef struct node *node_t;
typedef struct edge *edge_t;

node_t alloc_node(void* elem);
void* get_elem(node_t n);

size_t num_chld(node_t n);
size_t num_nattr(node_t n);
edge_t link_nodes(node_t from, node_t to, double w);

#endif /* LIBSRSBSNS_GRAPH_H */
