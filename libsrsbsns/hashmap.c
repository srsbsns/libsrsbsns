/* hashmap.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

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
	size_t count;

	size_t buckiter;
	size_t listiter;

	hmap_hash_fn hfn;
	hmap_eq_fn efn;
	hmap_keydup_fn keydupfn;
};

hmap_t
hmap_init(size_t bucketsz, hmap_hash_fn hfn, hmap_eq_fn efn, hmap_keydup_fn keydupfn)
{
	struct hmap *h = malloc(sizeof *h);
	if (!h)
		return NULL;

	h->buckiter = -1;
	h->bucketsz = bucketsz;
	h->count = 0;
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
	h->keydupfn = keydupfn;

	return h;
}

void
hmap_clear(hmap_t h)
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

	h->count = 0;
}

void
hmap_dispose(hmap_t h)
{
	if (!h)
		return;

	hmap_clear(h);

	free(h->keybucket);
	free(h->valbucket);
	free(h);
}

void
hmap_put(hmap_t h, const void *key, void *elem)
{
	if (!h || !key || !elem)
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
		ptrlist_insert(kl, 0, h->keydupfn(key));
		ptrlist_insert(vl, 0, elem);
		h->count++;
	} else
		ptrlist_replace(vl, i, elem);
}

void*
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

	free(ptrlist_get(kl, i));
	ptrlist_remove(kl, i);
	ptrlist_remove(vl, i);
	h->count--;
	return true;
}

size_t
hmap_count(hmap_t h)
{
	return !h ? 0 : h->count;
}

bool
hmap_first(hmap_t h, void **key, void **val)
{
	if (!h)
		return false;
	
	h->buckiter = 0;

	while (h->buckiter < h->bucketsz && (!h->keybucket[h->buckiter]
	    || ptrlist_count(h->keybucket[h->buckiter]) == 0))
		h->buckiter++;
	
	if (h->buckiter == h->bucketsz) {
		if (key)
			*key = NULL;
		if (val)
			*val = NULL;

		h->buckiter = -1;
		return true;
	}
	
	void *k = ptrlist_get(h->keybucket[h->buckiter], 0);
	void *v = ptrlist_get(h->valbucket[h->buckiter], 0);

	h->listiter = 1;

	if (key)
		*key = k;
	if (val)
		*val = v;
	
	return true;
}

bool
hmap_next(hmap_t h, void **key, void **val)
{
	if (!h || h->buckiter == -1)
		return false;
	
	void *k = ptrlist_get(h->keybucket[h->buckiter], h->listiter);

	if (!k) {
		h->buckiter++;
		h->listiter = 0;

		while (h->buckiter < h->bucketsz && (!h->keybucket[h->buckiter]
		    || ptrlist_count(h->keybucket[h->buckiter]) == 0))
			h->buckiter++;

		if (h->buckiter == h->bucketsz) {
			if (key)
				*key = NULL;
			if (val)
				*val = NULL;

			h->buckiter = -1;
			return true;
		}

		k = ptrlist_get(h->keybucket[h->buckiter], h->listiter);
	}

	void *v = ptrlist_get(h->valbucket[h->buckiter], h->listiter);

	h->listiter++;

	if (key)
		*key = k;
	if (val)
		*val = v;
	
	return true;
	
}

void
hmap_dump(hmap_t h, hmap_op_fn keyop, hmap_op_fn valop)
{
	#define M(X, A...) fprintf(stderr, X, ##A)
	M("===hashmap dump (count: %zu)===\n", h->count);
	if (!h)
		M("nullpointer...\n");

	for (size_t i = 0; i < h->bucketsz; i++) {
		if (h->keybucket[i] && ptrlist_count(h->keybucket[i])) {
			fprintf(stderr, "[%zu]: ", i);
			ptrlist_t kl = h->keybucket[i];
			ptrlist_t vl = h->valbucket[i];
			void *key = ptrlist_first(kl);
			void *val = ptrlist_first(vl);
			while (key) {
				keyop(key);
				fprintf(stderr, " --> ");
				valop(val);
				key = ptrlist_next(kl);
				val = ptrlist_next(vl);
			}
			fprintf(stderr, "\n");
		}
	}
	M("===end of hashmap dump===\n");
	#undef M
}

void
hmap_dumpstat(hmap_t h)
{
	size_t used = 0;
	size_t usedlen = 0;
	size_t empty = 0;
	for (size_t i = 0; i < h->bucketsz; i++) {
		if (h->keybucket[i]) {
			size_t c = ptrlist_count(h->keybucket[i]);
			if (c > 0) {
				used++;
				usedlen += c;
			} else
				empty++;
		}
	}

	fprintf(stderr, "hashmap stat: bucksz: %zu, used: %zu (%f%%) "
	    "avg listlen: %f\n", h->bucketsz, used,
	    ((double)used/h->bucketsz)*100.0, ((double)usedlen/used));
}
