/* common.c - (C) 2015, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "common.h"

char *
str_dup(const char *s)
{
	size_t len = strlen(s);
	char *r = malloc(len + 1);
	if (r)
		memcpy(r, s, len + 1);
	return r;
}
