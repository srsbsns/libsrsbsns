/* stack.c - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdlib.h>

#include <libsrsbsns/deque.h>
#include <libsrsbsns/stack.h>

struct stack{
	deque_t data;
};

stack_t
stack_init(size_t initsize)
{
	struct stack *s = malloc(sizeof *s);
	s->data = deque_init(initsize);
	return s;
}

void
stack_dispose(stack_t s)
{
	deque_dispose(s->data);
	free(s);
}

void
stack_clear(stack_t s)
{
	deque_clear(s->data);
}

size_t
stack_count(stack_t s)
{
	return deque_count(s->data);
}

void 
stack_push(stack_t s, void* data)
{
	deque_pushfront(s->data, data);
}

void*
stack_pop(stack_t s)
{
	return deque_popfront(s->data);
}

void*
stack_peek(stack_t s)
{
	return deque_peekfront(s->data);
}

void*
stack_first(stack_t s, bool back)
{
	return deque_first(s->data, back);
}

void*
stack_next(stack_t s, bool forwards)
{
	return deque_next(s->data, forwards);
}

