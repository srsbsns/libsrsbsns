#include <stdlib.h>
#include <libsrsbsns/bintree.h>

struct bt_node {
	void *data;
	struct bt_node *left;
	struct bt_node *right;
};

struct bintree {
	size_t count;
	bintree_cmp_fn cmpfn;
	struct bt_node *root;
};

static void bintree_rclear(struct bt_node *n);
static struct bt_node* bintree_nodefind(bintree_t t, void *data);
static void bintree_swapnode(struct bt_node *a, struct bt_node *b);
static void bintree_dumptoarray(struct bt_node *n, void **dest, size_t *i);
static void bintree_rbalance(bintree_t t, void **src, size_t start, size_t end);


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

size_t
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

	bintree_rclear(t->root);
	t->count = 0;
}

static void
bintree_rclear(struct bt_node *n)
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
		else
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

static struct bt_node*
bintree_nodefind(bintree_t t, void *data)
{
	if(!t)
		return 0;

	struct bt_node *n = t->root;

	while(n)
	{
		if(t->cmpfn(data, n->data) < 0)
			n = n->left;
		else if(t->cmpfn(data, n->data) > 0)
			n = n->right;
		else
			return n;
	}

	return NULL;
}

void*
bintree_find(bintree_t t, void* data)
{
	struct bt_node *res = bintree_nodefind(t, data);
	return res ? res->data : NULL;
}

bool
bintree_remove(bintree_t t, void *data)
{
	struct bt_node *n = bintree_nodefind(t, data);
	struct bt_node *tmp;
	if(!n->right && !n->left)
	{
		free(n);
		return true;
	}
	else if(n->right)
	{
		tmp = n->right;
		while(tmp->left)
			tmp = tmp->left;
	}
	else
	{
		tmp = n->left;
		while(tmp->right)
			tmp = tmp->right;
	}
	bintree_swapnode(tmp, n);
	free(tmp);
	t->count--;
	return true;
}

static void
bintree_swapnode(struct bt_node *a, struct bt_node *b)
{
	void *data = a->data;
	a->data = b->data;
	b->data = data;
}

static void
bintree_dumptoarray(struct bt_node *n, void **dest, size_t *i)
{
	if(!n)
		return

	bintree_dumptoarray(n->left, dest, i);
	dest[(*i)++] = n->data;
	bintree_dumptoarray(n->right, dest, i);
}

static void
bintree_rbalance(bintree_t t, void **src, size_t start, size_t end)
{
	size_t mid = (start+end)/2;
	bintree_insert(t, src[mid]);
	bintree_rbalance(t, src, start, mid-1);
	bintree_rbalance(t, src, mid+1, end);
}

void 
bintree_balance(bintree_t t)
{
	size_t n = t->count;
	void **arr = malloc(n * sizeof *arr);

	size_t i = 0;
	bintree_dumptoarray(t->root, arr, &i);
	bintree_clear(t);
	bintree_rbalance(t, arr, 0, i-1);
}
