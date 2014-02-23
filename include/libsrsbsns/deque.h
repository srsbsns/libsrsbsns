/* deque.h - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdbool.h>

typedef struct deque *deque_t;

deque_t deque_init(int initsize);
void deque_dispose(deque_t d);
void deque_clear(deque_t d);
size_t deque_count(deque_t d);
bool deque_pushfront(deque_t d, void* data);
bool deque_pushback(deque_t d, void* data);
void* deque_popfront(deque_t d);
void* deque_popback(deque_t d);