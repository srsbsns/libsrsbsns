/* stack.c - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */
#include <libsrsbsns/deque.h>

struct stack{
	deque_t data;
};

stack_t
stack_init(int initsize)
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