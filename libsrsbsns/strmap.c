/* strmap.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include <libsrsbsns/ptrlist.h>
#include <libsrsbsns/hashmap.h>
#include <libsrsbsns/strmap.h>


static bool
streq(const void *s1, const void *s2)
{
	return strcmp((const char*)s1, (const char*)s2) == 0;
}

static void*
strkeydup(const void *s)
{
	return strdup((const char*)s);
}

static size_t
strhash_small(const void *s)
{
	uint8_t res = 0xaa;
	uint8_t cur;
	bool shift = false;
	char *str = (char*)s;

	while ((cur = *str++)) {
		if ((shift = !shift))
			cur <<= 3;

		res ^= cur;
	}

	return res;
}

static size_t
strhash_mid(const void *s)
{
	uint16_t res = 0xaaaa;
	uint16_t cur = 0;
	int step = 0;
	const char *str = (const char*)s;
	uint8_t b;
	while ((b = *str++)) {
		cur |= ((uint16_t)b) << step*8;

		if (++step == 2) {
			step = 0;
			res ^= cur;
			cur = 0;
		}
	}

	if (step)
		res ^= cur;
	
	return res;

}

static size_t
strhash_large(const void *s)
{
	uint32_t res = 0xaaaaaaaa;
	uint32_t cur = 0;
	int step = 0;
	const char *str = (const char*)s;
	uint8_t b;
	while ((b = *str++)) {
		cur |= ((uint32_t)b) << step*8;

		if (++step == 4) {
			step = 0;
			res ^= cur;
			cur = 0;
		}
	}

	if (step)
		res ^= cur;
	
	return res;
}

smap_t
smap_init(size_t bucketsz)
{
	hmap_hash_fn fn = bucketsz <= 256 ? strhash_small :
	                  bucketsz <= 65536 ? strhash_mid : strhash_large;
	return hmap_init(bucketsz, fn, streq, strkeydup);
}

void
smap_clear(smap_t h)
{
	hmap_clear(h);
}

void
smap_dispose(smap_t h)
{
	hmap_dispose(h);
}

void
smap_put(smap_t h, const char *key, void *elem)
{
	hmap_put(h, key, elem);
}

void*
smap_get(smap_t h, const char *key)
{
	return hmap_get(h, key);
}

bool
smap_del(smap_t h, const char *key)
{
	return hmap_del(h, key);
}

size_t smap_count(smap_t h)
{
	return hmap_count(h);
}

