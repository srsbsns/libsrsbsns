/* deque.h - (C) 2014, Emir Marincic
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#include <stdbool.h>
#include <stddef.h>

typedef struct deque *deque_t;
typedef void (*deque_dump_fn)(void *elem);

deque_t deque_init(size_t initsize);
void deque_dispose(deque_t d);
void deque_clear(deque_t d);
size_t deque_count(deque_t d);
bool deque_pushfront(deque_t d, void* data);
bool deque_pushback(deque_t d, void* data);
bool deque_shrink(deque_t d, double countfac);
void* deque_popfront(deque_t d);
void* deque_popback(deque_t d);
void* deque_peekfront(deque_t d);
void* deque_peekback(deque_t d);
void* deque_first(deque_t d, bool front);
void* deque_next(deque_t d, bool forwards);
void deque_dump(deque_t d, deque_dump_fn dfn);
