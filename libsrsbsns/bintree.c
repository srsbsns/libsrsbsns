#include <stdlib.h>
#include <libsrsbsns/bintree.h>

struct bt_node {
	const void *data;
	struct bt_node *left;
	struct bt_node *right;
}

struct bintree {
	int count;
	bintree_cmp_fn cmpfn;
	struct bt_node *root;
};

bintree_t
bintree_init(bintree_cmp_fn cmpfn)
{
	struct bintree *t = malloc(sizeof *t);
	t->root = NULL;
	t->cmpfn = cmpfn;
	return t;
}

void
bintree_dispose(bintree_t t)
{
	bintree_clear(t);
	free(t);
}

int
bintree_count(bintree_t t)
{
	if(!t || !t->root)
		return 0;
	return t->count;
}

void
bintree_clear(bintree_t t)
{
	if(!t)
		return;

	bintree_rclear(&t->root);

}

static void
bintree_rclear(bt_node *n)
{
	if(n->left)
		bintree_rclear(n->left);
	if(n->right)
		bintree_rclear(n->right);
	free(n);
}

bool
bintree_insert(bintree_t t, void *data)
{
	if(!t)
		return false;

	struct bt_node *n = t->root; 

	while(n) {
		if(t->cmpfn(data, n->data) <= 0)
		{
			if(n->left)
				n = n->left;
			else
			{
				n->left = malloc(sizeof *n->left);
				n = n->left;
				break;
			}
		}
		else if(t->cmpfn(data, n->data) > 0)
		{
			if(n->right)
				n = n->right;
			else
			{
				n->right = malloc(sizeof *n->right);
				n = n->right;
				break;
			}
		}
	}
	if(!n)
		return false;
	n->data = data;
	t->count++;
	return true;
}

static bt_node*
bintree_nodefind(bintree_t t, void *data)
{
	if(!t)
		return 0;

	struct bt_node *n = t->root;

	while(n)
	{
		if(!n)
			return NULL;
		if(t->cmpfn(data, n->data) < 0)
			n = n->left;
		else if(t->cmpfn(data, n->data) > 0)
			n = n->right;
		else
			return n;
	}
}

void*
bintree_find(bintree_t t, void* data)
{
	return bintree_nodefind(t, data)->data;
}

bool
bintree_remove(bintree_t t, void *data)
{
	struct bt_node *n = bintree_nodefind(t, data);
	if(!n->right && !n->left)
	{
		free(n);
		return true;
	}
	else if(n->right)
	{
		struct bt_node *tmp = n->right;
		while(tmp->left)
			tmp = tmp->left;
	}
	else
	{
		struct bt_node *tmp = n->left;
		while(tmp->right)
			tmp = tmp->right;
	}
	bintree_swapnode(tmp, n);
	free(tmp);
	t->count++;
	return true;
}

void
bintree_swapnode(bt_node *a, bt_node *b)
{
	void *data = a->data;
	a->data = b->data;
	b->data = data;
}