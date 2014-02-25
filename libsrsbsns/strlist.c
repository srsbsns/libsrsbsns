/* strlist.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

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
	ptrlist_dispose(l);
}

size_t 
slist_count(slist_t l)
{
	return ptrlist_count(l);
}

void 
slist_clear(slist_t l)
{
	void *d = slist_first(l);
	do {
		free(d);
	} while ((d = slist_next(l)));
	ptrlist_clear(l);
}

bool
slist_insert(slist_t l, size_t i, const char *data)
{
	if (!data)
		return false;

	return ptrlist_insert(l, i, strdup(data));
}

bool
slist_replace(slist_t l, size_t i, const char *data)
{
	if (!data)
		return false;

	void *d = ptrlist_get(l, i);
	free(d);
	return ptrlist_replace(l, i, strdup(data));
}

bool
slist_remove(slist_t l, size_t i)
{
	void *d = ptrlist_get(l, i);
	free(d);
	return ptrlist_remove(l, i);
}

char*
slist_get(slist_t l, size_t i)
{
	return ptrlist_get(l, i);
}

ssize_t
slist_findeq(slist_t l, const char *needle)
{
	size_t cnt = slist_count(l);
	for (size_t i = 0; i < cnt; i++) {
		const char *c = slist_get(l, i);
		if (streq(c, needle))
			return i;
	}

	return -1;
}

static void
strdump(const void *s)
{
	fprintf(stderr, "%s", (const char*)s);
}

void
slist_dump(slist_t l)
{
	ptrlist_dump(l, strdump);
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
