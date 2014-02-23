/* deque.c - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libsrsbsns/deque.h>

struct deque {
	size_t nelem;
	void **data;
	size_t back;
	size_t front;
};

static bool deque_grow(deque_t d);

deque_t
deque_init(size_t initsize)
{
	struct deque *d = malloc(sizeof *d);
	d->data = malloc(sizeof *d->data * initsize);
	d->nelem = initsize;
	d->back = d->front = d->nelem/2;
	return d;
}

void
deque_dispose(deque_t d)
{
	if (!d)
		return;

	free(d->data);
	free(d);
}

void
deque_clear(deque_t d)
{
	if (!d)
		return;

	d->back = d->front = d->nelem/2;
}

size_t
deque_count(deque_t d)
{
	return d->front - d->back;
}

bool
deque_pushfront(deque_t d, void* data)
{
	if(!d)
		return false;

	while (d->front >= d->nelem)
		if (!deque_grow(d))
			return false;
		
	d->data[d->front++] = data;
	return true;
}

bool
deque_pushback(deque_t d, void* data)
{
	if(!d)
		return false;

	while (d->back == 0)
		if (!deque_grow(d))
			return false;

	d->data[--d->back] = data;
	return true;
}

static bool
deque_grow(deque_t d)
{
	size_t newsz = d->nelem * 2;
	if (!newsz)
		newsz = 1;

	void **newloc = malloc(newsz * sizeof *newloc);
	if(!newloc)
		return false;

	size_t newback = (newsz - deque_count(d))/2;
	size_t newfront = newback;
	for (size_t i = d->back; i < d->front; i++)
		newloc[newfront++] = d->data[i];
	
	//size_t offset = (newsz * sizeof *d->data - deque_count(d))/2;
	//memcpy(newloc + offset, d->data, d->nelem * sizeof *d->data);

	free(d->data);
	d->data = newloc;
	d->back = newback;
	d->front = newfront;
	//d->front = offset + deque_count(d);
	//d->back = offset;
	d->nelem = newsz;
	return true;
}

void
deque_shrink(deque_t d)
{
	size_t numelem = d->front - d->back + 1;
	d->data = realloc(d->data, sizeof d->data * numelem);
	d->front = numelem;
	d->back = 0;
}

void*
deque_popfront(deque_t d)
{
	if(!d || d->back == d->front)
		return NULL;

	void *ret = d->data[--d->front];

	if (d->front == d->back)
		deque_clear(d); //recenter

	return ret;
}

void*
deque_popback(deque_t d)
{
	if(!d || d->back == d->front) 
		return NULL;

	void *ret = d->data[d->back++];

	if (d->front == d->back)
		deque_clear(d); //recenter

	return ret;
}

void*
deque_peekfront(deque_t d)
{
	return d->data[d->front];
}

void*
deque_peekback(deque_t d)
{
	return d->data[d->back];
}

void
deque_dump(deque_t d, deque_dump_fn dfn)
{
	if (!d) {
		fprintf(stderr, "(not a) deque %p\n", d);
		return;
	}
		
	fprintf(stderr, "deque %p [nelem: %zu, count: %zu front: %zu, back: %zu]:\n",
	    d, d->nelem, deque_count(d), d->front, d->back);
	
	if (d->back == d->front)
		fputs("[deque is empty]", stderr);
	else
		for (size_t i = d->back; i < d->front; i++) {
			fputs(i == d->back ? "data: " : ", ", stderr);
			dfn(d->data[i]);
		}

	fputs("\n", stderr);
}
