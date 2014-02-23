/* deque.c - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libsrsbsns/deque.h>

struct deque {
	size_t size;
	void **data;
	size_t back;
	size_t front;
};

static bool deque_grow(deque_t d);

deque_t
deque_init(int initsize)
{
	struct deque *d = malloc(sizeof *d);
	d->data = malloc(sizeof *d->data * initsize);
	d->size = initsize;
	d->back = d->front = d->size/2;
	d->front--;
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

	d->back = d->front = d->size/2;
	d->front--;
}

size_t
deque_count(deque_t d)
{
	return d->back > d->front ? 0 : d->front - d->back + 1;
}

bool
deque_pushfront(deque_t d, void* data)
{
	if(!d)
		return false;

	if(d->front + 1 > d->size)
		if(!deque_grow(d))
			return false;
	d->front++;
	d->data[d->front] = data;
	return true;
}

bool
deque_pushback(deque_t d, void* data)
{
	if(!d)
		return false;

	if(d->back - 1 < 0)
		if(!deque_grow(d))
			return false;
	d->back--;
	d->data[d->back] = data;
	return true;
}

static bool
deque_grow(deque_t d)
{
	void **newloc = malloc(sizeof **newloc * d->size*2);
	if(!newloc)
		return false;
	size_t numelem = d->front - d->back + 1;
	size_t offset = (d->size * 2 * sizeof d->data - numelem)/2;
	memcpy(newloc + offset, d->data, d->size * sizeof d->data);
	free(d->data);
	d->data = newloc;
	d->front = offset + numelem;
	d->back = offset;
	d->size *= 2;
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
	if(!d)
		return NULL;

	void *ret = d->data[d->front];
	d->data[d->front] = NULL;
	d->front--;
	return ret;
}

void*
deque_popback(deque_t d)
{
	if(!d) 
		return NULL;

	void *ret = d->data[d->back];
	d->data[d->back] = NULL;
	d->back++;
	return ret;
}

void
deque_dump(deque_t d, deque_dump_fn dfn)
{
	fprintf(stderr, "deque %p [size: %zu, front: %zu, back: %zu]:\n",
	    d, d->size, d->front, d->back);
	
	if (d->back > d->front)
		fputs("[deque is empty]", stderr);
	else
		for (size_t i = d->back; i <= d->front; i++) {
			fputs(i == d->back ? "data: " : ", ", stderr);
			dfn(d->data[i]);
		}

	fputs("\n", stderr);
}
