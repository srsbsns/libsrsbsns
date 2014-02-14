/* ptrlist.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_PTRLIST_H
#define LIBSRSBSNS_PTRLIST_H 1

/* simple and stupid single linked list with void* data elements */

#include <stdbool.h>

typedef struct ptrlist *ptrlist_t;
typedef bool (*ptrlist_find_fn)(void *e);

/* alloc/dispose/count/clear */
ptrlist_t ptrlist_init(void);
void ptrlist_dispose(ptrlist_t l);
void ptrlist_clear(ptrlist_t l);
size_t ptrlist_count(ptrlist_t l);

/* insert/remove/get by index */
bool ptrlist_insert(ptrlist_t l, size_t i, void *data);
bool ptrlist_remove(ptrlist_t l, size_t i);

/* linear search */
ssize_t ptrlist_findraw(ptrlist_t, void *data);
ssize_t ptrlist_findfn(ptrlist_t, ptrlist_find_fn fndfn);

/* iteration */
void* ptrlist_first(ptrlist_t l);
void* ptrlist_next(ptrlist_t l);
void ptrlist_dump(ptrlist_t l);

#endif /* LIBSRSBSNS_PTRLIST_H */