/* dynarr.c - (C) 2014, Timo Buhrmester
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libsrsbsns/dynarr.h>

struct dynarr {
	size_t nelem; //number of elements allocated
	size_t nused; //apparent size to the outside world (<= nelem)
	void **data;
	bool nulling;
};

static void provide_index(dynarr_t d, size_t index);
static bool dynarr_resize(dynarr_t d, size_t newsz);
static void nullrange(void **data, size_t off, size_t num);

dynarr_t
dynarr_init(size_t initsize, bool nulling)
{
	struct dynarr *d = malloc(sizeof *d);
	d->data = malloc(sizeof *d->data * initsize);
	d->nelem = initsize;
	d->nused = initsize;
	if ((d->nulling = nulling))
		nullrange(d->data, 0, initsize);
	return d;
}

void
dynarr_dispose(dynarr_t d)
{
	if (!d)
		return;

	free(d->data);
	free(d);
}

void
dynarr_clear(dynarr_t d)
{
	if (!d)
		return;

	d->nused = 0;
}

void
dynarr_truncate(dynarr_t d, size_t fromindex)
{
	if (!d)
		return;

	d->nused = fromindex;
	if (d->nulling)
		nullrange(d->data, fromindex, d->nelem - fromindex);
}

size_t
dynarr_count(dynarr_t d)
{
	return d->nused;
}

bool
dynarr_push(dynarr_t d, void* data)
{
	provide_index(d, d->nused);
	d->data[d->nused++] = data;
	return true;
}

static void
provide_index(dynarr_t d, size_t index)
{
	if (index >= d->nelem) {
		size_t nsz = 1 << (!index ? 0 : ((size_t)log2(index) + 1));
		
		if (!dynarr_resize(d, nsz)) {
			fprintf(stderr, "dynarr_resize failed!\n");
			exit(EXIT_FAILURE);
		}
	}

	if (index > d->nused)
		d->nused = index + 1;
}

void*
dynarr_get(dynarr_t d, size_t index)
{
	provide_index(d, index);
	return d->data[index];
}

void
dynarr_put(dynarr_t d, size_t index, void *elem)
{
	provide_index(d, index);
	d->data[index] = elem;
}

static void
nullrange(void **data, size_t off, size_t num)
{
	for (size_t i = 0; i < num; i++)
		data[i+off] = NULL;
}

static bool
dynarr_resize(dynarr_t d, size_t newsz)
{
	void **newloc = malloc(newsz * sizeof *newloc);
	if (!newloc)
		return false;
	
	size_t min = newsz < d->nused ? newsz : d->nused;

	memcpy(newloc, d->data, min * sizeof *newloc);
	if (d->nulling)
		nullrange(newloc, min, newsz - min);

	free(d->data);
	d->data = newloc;
	d->nelem = newsz;
	d->nused = min;

	return true;
}

bool
dynarr_shrink(dynarr_t d, double countfac)
{
	if (!d || countfac < 1.0)
		return false;
		
	return dynarr_resize(d, (size_t)(countfac * d->nused));
}

void
dynarr_dump(dynarr_t d, dynarr_dump_fn dfn)
{
	if (!d) {
		fprintf(stderr, "(not a) dynarr %p\n", d);
		return;
	}

	fprintf(stderr, "dynarr %p [nelem: %zu, count: %zu ]:\n",
	    d, d->nelem, dynarr_count(d));

	if (d->nused == 0)
		fputs("[dynarr is empty]", stderr);
	else
		for (size_t i = 0; i < d->nused; i++) {
			fputs(!i ? "data: " : ", ", stderr);
			dfn(d->data[i]);
		}

	fputs("\n", stderr);
}
