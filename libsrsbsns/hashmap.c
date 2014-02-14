/* hashmap.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <libsrsbsns/ptrlist.h>
#include <libsrsbsns/hashmap.h>

struct hmap {
	ptrlist_t *keybucket;
	ptrlist_t *valbucket;
	size_t bucketsz;

	hmap_hash_fn hfn;
	hmap_eq_fn efn;
};

hmap_t
hmap_init(size_t bucketsz, hmap_hash_fn hfn, hmap_eq_fn efn)
{
	struct hmap *h = malloc(sizeof *h);
	if (!h)
		return NULL;

	h->bucketsz = bucketsz;
	h->keybucket = malloc(h->bucketsz * sizeof *h->keybucket);
	if (!h->keybucket) {
		free(h);
		return NULL;
	}
	h->valbucket = malloc(h->bucketsz * sizeof *h->valbucket);
	if (!h->valbucket) {
		free(h->keybucket);
		free(h);
		return NULL;
	}

	for (size_t i = 0; i < h->bucketsz; i++) {
		h->valbucket[i] = NULL;
		h->keybucket[i] = NULL;
	}

	h->hfn = hfn;
	h->efn = efn;

	return h;
}

void
hmap_dispose(hmap_t h)
{
	if (!h)
		return;

	for (size_t i = 0; i < h->bucketsz; i++) {
		if (h->keybucket[i]) {
			ptrlist_dispose(h->keybucket[i]);
			h->keybucket[i] = NULL;
		}
		if (h->valbucket[i]) {
			ptrlist_dispose(h->valbucket[i]);
			h->valbucket[i] = NULL;
		}
	}
	
	free(h->keybucket);
	free(h->valbucket);
	free(h);
}

void
hmap_put(hmap_t h, const void *key, const void *elem)
{
	if (!h)
		return;

	size_t ind = h->hfn(key) % h->bucketsz;
	
	ptrlist_t kl = h->keybucket[ind];
	ptrlist_t vl = h->valbucket[ind];
	if (!kl) {
		kl = h->keybucket[ind] = ptrlist_init();
		vl = h->valbucket[ind] = ptrlist_init();
	}

	ssize_t i = ptrlist_findeqfn(kl, h->efn, key);
	if (i == -1) {
		ptrlist_insert(kl, 0, key);
		ptrlist_insert(vl, 0, elem);
	} else
		ptrlist_replace(vl, i, elem);
}

const void*
hmap_get(hmap_t h, const void *key)
{
	if (!h)
		return NULL;

	size_t ind = h->hfn(key) % h->bucketsz;
	
	ptrlist_t kl = h->keybucket[ind];
	if (!kl)
		return NULL;
	ptrlist_t vl = h->valbucket[ind];

	ssize_t i = ptrlist_findeqfn(kl, h->efn, key);
	
	return i == -1 ? NULL : ptrlist_get(vl, i);
}

bool
hmap_del(hmap_t h, const void *key)
{
	if (!h)
		return false;

	size_t ind = h->hfn(key) % h->bucketsz;
	
	ptrlist_t kl = h->keybucket[ind];
	if (!kl)
		return false;
	ptrlist_t vl = h->valbucket[ind];

	ssize_t i = ptrlist_findeqfn(kl, h->efn, key);
	
	if (i == -1)
		return false;

	ptrlist_remove(kl, i);
	ptrlist_remove(vl, i);
	return true;
}

void
hmap_dump(hmap_t h)
{
	#define M(X, A...) fprintf(stderr, X, ##A)
	M("===hashmap dump===\n");
	if (!h)
		M("nullpointer...\n");

	for (size_t i = 0; i < h->bucketsz; i++) {
		if (h->keybucket[i] && ptrlist_count(h->keybucket[i])) {
			fprintf(stderr, "[%zu] keys: ", i);
			ptrlist_dump(h->keybucket[i]);
			fprintf(stderr, "[%zu] vals: ", i);
			ptrlist_dump(h->valbucket[i]);
		}
	}
	M("===end of hashmap dump===\n");
	#undef M
}
