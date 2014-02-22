/* deque.c - (C) 2012, Emir Marincic
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
	return d;
}

static bool deque_grow(deque_t d);

bool
deque_pushfront(deque_t d, void* data)
{
	if(!d)
		return false;

	if(!d->front)  //Deque empty
		 d->data[d->size/2] = data;
	else {
		if(!d->data[d->front + 1])
			if(!deque_grow(d))
				return false;
		d->front++;
		d->data[d->front] = data;
	}
	return true;
}

bool
deque_pushback(deque_t d, void* data)
{
	if(!d)
		return false;

	if(!d->back)  //Deque empty
		 d->data[d->size/2] = data;
	 else {
		if(!d->data[d->back + 1])
			if(!deque_grow(d))
				return false;
		d->back++;
		d->data[d->back] = data;
	}
	return true;
}

static bool
deque_grow(deque_t d)
{
	if(!d)
		return false;
	void **newloc = malloc(sizeof **newloc * d->size*2);
	if(!newloc)
		return false;
	size_t offset = (d->size * 2 * sizeof d->data - d->front + d->back)/2;
	memcpy(newloc + offset, d->data, d->size * sizeof d->data);
	free(d->data);
	d->data = newloc;
}

void*
deque_popfront(deque_t d)
{
	if(!d)
		return NULL;

	if(!d->front)  //Deque empty
		return NULL;
	else {
		void *ret = d->data[d->front];
		d->data[d->front] = NULL;
		d->front--;
		return ret;
	}
}

void*
deque_popback(deque_t d)
{
	if(!d)
		return NULL;

	if(!d->back)  //Deque empty
		return NULL;
	else {
		void *ret = d->data[d->back];
		d->data[d->back] = NULL;
		d->back--;
		return ret;
	}
}