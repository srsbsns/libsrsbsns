/* stack.h - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_STACK_H
#define LIBSRSBSNS_STACK_H 1

typedef struct stack* stack_t;

stack_t stack_init(size_t initsize);
void stack_dispose(stack_t s);
void stack_clear(stack_t s);
void stack_push(stack_t s, void* data);
void* stack_pop(stack_t s);
void* stack_peek(stack_t s);

#endif /* LIBSRSBSNS_STACK_H */
