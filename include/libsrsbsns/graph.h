/* graph.h - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_GRAPH_H
#define LIBSRSBSNS_GRAPH_H 1

#define TRAV_BFS 0
#define TRAV_DFS 1

#include <sys/types.h>

typedef struct node *node_t;
typedef struct edge *edge_t;

typedef void (*node_op_fn)(node_t n, void *user);
typedef void (*edge_op_fn)(edge_t n, void *user);

node_t alloc_node(void* elem);
void* get_elem(node_t n);
void set_elem(node_t n, void *elem);
size_t fan_out(node_t n);
size_t fan_in(node_t n) ;
size_t num_nattr(node_t n);
size_t num_eattr(edge_t e);
edge_t link_node(node_t from, node_t to, double w);
void unlink_node(node_t from, node_t to);
edge_t find_edge(node_t from, node_t to);
void drop_edge(edge_t e);
void traverse(node_t n, int travtype, node_op_fn nf, edge_op_fn ef, void *user);

#endif /* LIBSRSBSNS_GRAPH_H */
