/* bintree.c - (C) 2014, Emir Marincic, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#include <stdlib.h>
#include <stdio.h>
#include <libsrsbsns/bintree.h>

struct bt_node {
	void *data;
	struct bt_node *left;
	struct bt_node *right;
	struct bt_node *parent;
};

struct bintree {
	size_t count;
	bintree_cmp_fn cmpfn;
	struct bt_node *root;

	struct bt_node *iter;
	int travtype;
};

static void bintree_rclear(struct bt_node *n);
static struct bt_node* bintree_nodefind(bintree_t t, void *data);
static void bintree_rbalance(bintree_t t, void **src, size_t start, size_t end);
static void rtrav_preorder(struct bt_node *n, void **dest, size_t *i);
static void rtrav_inorder(struct bt_node *n, void **dest, size_t *i);
static void rtrav_postorder(struct bt_node *n, void **dest, size_t *i);
static void* first_preorder(bintree_t t);
static void* first_inorder(bintree_t t);
static void* first_postorder(bintree_t t);
static void* next_preorder(bintree_t t);
static void* next_inorder(bintree_t t);
static void* next_postorder(bintree_t t);



bintree_t
bintree_init(bintree_cmp_fn cmpfn)
{
	struct bintree *t = malloc(sizeof *t);
	t->root = NULL;
	t->cmpfn = cmpfn;
	t->iter = NULL;
	return t;
}

void
bintree_dispose(bintree_t t)
{
	bintree_clear(t);
	free(t);
}

static void
discoverheight(struct bt_node *n, size_t depth, size_t *maxdepth)
{
	if (depth > *maxdepth)
		*maxdepth = depth;
	
	if (n->left)
		discoverheight(n->left, depth+1, maxdepth);
	if (n->right)
		discoverheight(n->right, depth+1, maxdepth);
}

size_t
bintree_height(bintree_t t)
{
	if (!t || !t->root)
		return 0;

	size_t max = 1;
	discoverheight(t->root, 1, &max);

	return max;
}

size_t
bintree_count(bintree_t t)
{
	if (!t || !t->root)
		return 0;
	return t->count;
}

void
bintree_clear(bintree_t t)
{
	if (!t)
		return;

	bintree_rclear(t->root);
	t->count = 0;
	t->root = NULL;
}

static void
bintree_rclear(struct bt_node *n)
{
	if (!n)
		return;

	bintree_rclear(n->left);
	bintree_rclear(n->right);
	free(n);
}

bool
bintree_insert(bintree_t t, void *data)
{
	if (!t || !data)
		return false;

	struct bt_node *n = t->root;

	if (!n) { //special case: tree is empty
		t->root = malloc(sizeof *t->root);
		t->root->parent = NULL;
		t->root->data = data;
		t->root->left = t->root->right = NULL;
		t->count = 1;
		return true;
	}

	while (n) {
		int res = t->cmpfn(data, n->data);
		if (res < 0) {
			if (n->left)
				n = n->left;
			else {
				n->left = malloc(sizeof *n->left);
				n->left->parent = n;
				n = n->left;
				n->left = n->right = NULL;
				t->count++;
				break;
			}
		} else if (res > 0) {
			if (n->right)
				n = n->right;
			else {
				n->right = malloc(sizeof *n->right);
				n->right->parent = n;
				n = n->right;
				n->left = n->right = NULL;
				t->count++;
				break;
			}
		} else
			break;
	}
	if (!n)
		return false;
	n->data = data;
	return true;
}

static struct bt_node*
bintree_nodefind(bintree_t t, void *data)
{
	if (!t)
		return 0;

	struct bt_node *n = t->root;

	while (n) {
		if (t->cmpfn(data, n->data) < 0)
			n = n->left;
		else if (t->cmpfn(data, n->data) > 0)
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
	if (!t)
		return false;

	struct bt_node *n = bintree_nodefind(t, data);

	if (!n)
		return false;

	bool root = !n->parent;
	bool left = !root && n->parent->left == n;

	if (!n->right && !n->left) {
		n->left = n->right = NULL;
		if (root)
			t->root = NULL;
		else if (left)
			n->parent->left = NULL;
		else
			n->parent->right = NULL;
	} else if (!n->left || !n->right) {
		struct bt_node *child = !n->left ? n->right : n->left;
		if (root) {
			t->root = child;
			child->parent = NULL;
		} else {
			if (left)
				n->parent->left = child;
			else
				n->parent->right = child;
			child->parent = n->parent;
		}
	} else {
		struct bt_node *maxnode = n->left;
		while (maxnode->right)
			maxnode = maxnode->right;

		void *tmp = maxnode->data;
		maxnode->data = n->data;
		n->data = tmp;

		if (maxnode->parent->left == maxnode)
			maxnode->parent->left = NULL;
		else if (maxnode->parent->right == maxnode)
			maxnode->parent->right = NULL;
		else {
			fprintf(stderr, "WAT!\n");
			exit(2);
		}

		n = maxnode;
	}

	free(n);

	t->count--;
	return true;
}

static void
bintree_rbalance(bintree_t t, void **src, size_t start, size_t end)
{
	size_t mid = (start+end)/2;
	bintree_insert(t, src[mid]);
	if (start < mid)
		bintree_rbalance(t, src, start, mid-1);
	
	if (end > mid)
		bintree_rbalance(t, src, mid+1, end);
}

void
bintree_balance(bintree_t t)
{
	size_t n = t->count;
	void **arr = malloc(n * sizeof *arr);

	if (!bintree_collect(t, arr, TRAV_INORDER))
		return;

	bintree_clear(t);
	bintree_rbalance(t, arr, 0, n-1);
}

void*
bintree_first(bintree_t t, int travtype)
{
	t->travtype = travtype;
	t->iter = NULL;

	switch (t->travtype) {
	case TRAV_PREORDER:
		return first_preorder(t);
	case TRAV_INORDER:
		return first_inorder(t);
	case TRAV_POSTORDER:
		return first_postorder(t);
	default:
		return NULL;
	}

}

void*
bintree_next(bintree_t t)
{
	switch (t->travtype) {
	case TRAV_PREORDER:
		return next_preorder(t);
	case TRAV_INORDER:
		return next_inorder(t);
	case TRAV_POSTORDER:
		return next_postorder(t);
	default:
		return NULL;
	}
}

static void*
first_preorder(bintree_t t)
{
	t->iter = t->root;
	return t->iter->data;
}

static void*
first_inorder(bintree_t t)
{
	t->iter = t->root;
	while (t->iter->left)
		t->iter = t->iter->left;

	return t->iter->data;
}

static void*
first_postorder(bintree_t t)
{
	t->iter = t->root;
	while (t->iter->left || t->iter->right) {
		if (t->iter->left)
			t->iter = t->iter->left;
		else
			t->iter = t->iter->right;
	}

	return t->iter->data;
}

static void*
next_preorder(bintree_t t)
{
	if (t->iter->left)
		t->iter = t->iter->left;
	else if (t->iter->right)
		t->iter = t->iter->right;
	else { /* we're a leaf node */
		for (;;) {
			/* if we're a right child, go up until we're not */
			while (t->iter->parent
			    && t->iter->parent->right == t->iter)
				t->iter = t->iter->parent;

			if (!t->iter->parent) //we're at root; done.
				return t->iter = NULL;

			/* now we must be a left child.
			 * if parent has no right child, go up one level,
			 * then repeat procedure */
			if (t->iter->parent && !t->iter->parent->right) {
				t->iter = t->iter->parent;
				continue;
			}

			if (!t->iter->parent) //we're at root; done.
				return t->iter = NULL;

			/* finally, we're a left child with a parent
			 * having a right child -- this is our node. */
			t->iter = t->iter->parent->right;
			break;
		}
	}

	return t->iter->data;
}

static void*
next_inorder(bintree_t t)
{
	bool root = !t->iter->parent;
	bool left = !root && t->iter->parent->left == t->iter;

	if (t->iter->right) {
		t->iter = t->iter->right;
		while (t->iter->left)
			t->iter = t->iter->left;
	} else {
		if (root)
			return t->iter = NULL; //done

		/* go up until we're at a node at which we arrived
		 * from the left side */
		struct bt_node *prev = NULL;
		do {
			prev = t->iter;
			t->iter = t->iter->parent;
			left = t->iter->left == prev;
		} while (!left && t->iter->parent);

		if (!t->iter->parent && !left)
			return t->iter = NULL; //at root; done
	}

	return t->iter->data;
}

static void*
next_postorder(bintree_t t)
{
	bool root = !t->iter->parent;
	bool left = !root && t->iter->parent->left == t->iter;


	if (root)
		return t->iter = NULL; //done

	t->iter = t->iter->parent;

	if (left && t->iter->right) {
		t->iter = t->iter->right;
		while (t->iter->left || t->iter->right) {
			if (t->iter->left)
				t->iter = t->iter->left;
			else
				t->iter = t->iter->right;
		}
	}

	return t->iter->data;
}

static void
rtrav_preorder(struct bt_node *n, void **dest, size_t *i)
{
	if (!n)
		return;

	dest[(*i)++] = n->data;
	rtrav_preorder(n->left, dest, i);
	rtrav_preorder(n->right, dest, i);
}

static void
rtrav_inorder(struct bt_node *n, void **dest, size_t *i)
{
	if (!n)
		return;

	rtrav_inorder(n->left, dest, i);
	dest[(*i)++] = n->data;
	rtrav_inorder(n->right, dest, i);
}

static void
rtrav_postorder(struct bt_node *n, void **dest, size_t *i)
{
	if (!n)
		return;

	rtrav_postorder(n->left, dest, i);
	rtrav_postorder(n->right, dest, i);
	dest[(*i)++] = n->data;
}


bool
bintree_collect(bintree_t t, void **dest, int travmode)
{
	if (!t)
		return false;

	if (!t->root)
		return true;

	size_t i = 0;

	switch (travmode) {
	case TRAV_PREORDER:
		rtrav_preorder(t->root, dest, &i);
		break;
	case TRAV_INORDER:
		rtrav_inorder(t->root, dest, &i);
		break;
	case TRAV_POSTORDER:
		rtrav_postorder(t->root, dest, &i);
		break;
	default:
		return false;
	}

	return true;
}

static void
bintree_rdump(struct bt_node *n, int depth, bintree_dump_fn df)
{
	if (!n)
		return;

	bintree_rdump(n->left, depth+1, df);
	for (int i = 0; i < depth; i++)
		fputs("  ", stderr);
	fputs("``", stderr);
	df(n->data);
	fprintf(stderr, "'' [%12p (p:%12p: ``", n, n->parent);
	if (n->parent)
		df(n->parent->data);
	else
		fprintf(stderr, "(root)");
	fprintf(stderr, "'')]");
	fputs("\n", stderr);
	bintree_rdump(n->right, depth+1, df);
}

void
bintree_dump(bintree_t t, bintree_dump_fn df)
{
	fprintf(stderr, "bintree %12p:\n", t);
	bintree_rdump(t->root, 0, df);
}
