/* strlist.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libsrsbsns/strlist.h>

static bool
streq(const char *s1, const char *s2)
{
	return strcmp(s1, s2) == 0;
}

slist_t
slist_init(void)
{
	return ptrlist_init();
}

void 
slist_dispose(slist_t l)
{
	return ptrlist_dispose(l);
}

size_t 
slist_count(slist_t l)
{
	return ptrlist_count(l);
}

void 
slist_clear(slist_t l)
{
	return ptrlist_clear(l);
}

bool
slist_insert(slist_t l, size_t i, const char *data)
{
	return ptrlist_insert(l, i, data);
}

bool
slist_replace(slist_t l, size_t i, const char *data)
{
	return ptrlist_replace(l, i, data);
}

bool
slist_remove(slist_t l, size_t i)
{
	return ptrlist_remove(l, i);
}

char*
slist_get(slist_t l, size_t i)
{
	return ptrlist_get(l, i);
}

ssize_t
slist_findraw(slist_t l, const char *data)
{
	return ptrlist_findraw(l, data);
}

ssize_t
slist_findeq(slist_t l, const char *needle)
{
	return ptrlist_findeqfn(l, streq, needle);
}

void
slist_dump(slist_t l)
{
	return ptrlist_dump(l);
}

char*
slist_first(slist_t l)
{
	return ptrlist_first(l);
}

char*
slist_next(slist_t l)
{
	return ptrlist_next(l);
}
