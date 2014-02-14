/* strlist.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_STRLIST_H
#define LIBSRSBSNS_STRLIST_H 1

#include "ptrlist.h"

#include <stdbool.h>

typedef ptrlist_t slist_t;

typedef bool (*slist_find_fn)(const char *e);
typedef bool (*slist_eq_fn)(const char *e1, const char *e2);

/* alloc/dispose/count/clear */
slist_t slist_init(void);
void slist_dispose(slist_t l);
void slist_clear(slist_t l);
size_t slist_count(slist_t l);

/* insert/remove/get by index */
bool slist_insert(slist_t l, size_t i, const char *data);
bool slist_replace(slist_t l, size_t i, const char *data);
bool slist_remove(slist_t l, size_t i);
const char* slist_get(slist_t l, size_t i);

/* linear search */
ssize_t slist_findraw(slist_t, const char *data);
ssize_t slist_findeq(slist_t l, const char *needle);

/* iteration */
const char* slist_first(slist_t l);
const char* slist_next(slist_t l);
void slist_dump(slist_t l);

#endif /* LIBSRSBSNS_STRLIST_H */
