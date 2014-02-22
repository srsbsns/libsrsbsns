/* deque.h - (C) 2012, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

typedef struct deque *deque_t;

deque_t deque_init(int initsize);
bool deque_pushfront(deque_t d, void* data);
bool deque_pushback(deque_t d, void* data);
void* deque_popfront(deque_t d);
void* deque_popback(deque_t d);