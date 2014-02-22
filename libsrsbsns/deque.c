/* deque.c - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdlib.h>
#include <libsrsbsns/deque.h>

struct deque {
	size_t size;
	void **data;
	size_t back;
	size_t front;
};

deque_t
deque_init(int initsize)
{
	struct deque *d = malloc(sizeof *d);
	d->data = malloc(sizeof d->data * initsize);
	d->size = initsize;
	d->back = d->front = d->size/2;
	d->front--;
	return d;
}

static bool deque_grow(deque_t d);

bool
deque_pushfront(deque_t d, void* data)
{
	if(!d)
		return false;

	if(!d->data[d->front + 1])
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

	if(!d->data[d->back - 1])
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
	size_t numelem = d->front - d->back;
	size_t offset = (d->size * 2 * sizeof d->data - numelem)/2;
	memcpy(newloc + offset, d->data, d->size * sizeof d->data);
	free(d->data);
	d->data = newloc;
	d->front = offset + numelem;
	d->back = offset;
	d->size *= 2;
	return true;
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