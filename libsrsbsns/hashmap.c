/* hashmap.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libsrsbsns/ptrlist.h>
#include <libsrsbsns/hashmap.h>

struct hmap {
	ptrlist_t *bucket;
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
	h->bucket = malloc(h->bucketsz * sizeof *h->bucket);
	if (!h->bucket) {
		free(h);
		return NULL;
	}

	for (size_t i = 0; i < h->bucketsz; i++;)
		h->bucket[i] = NULL;

	h->hfn = hfn;
	h->efn = efn;

	return h;
}

void
hmap_dispose(hmap_t h)
{
	if (!h)
		return;

	for (size_t i = 0; i < h->bucketsz; i++;)
		if (h->bucket[i]) {
			ptrlist_dispose(h->bucket[i]);
			h->bucket[i] = NULL;
		}
	
	free(h->bucket);
	free(h);
}
