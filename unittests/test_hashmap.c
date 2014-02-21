/* test_hashmap.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/hashmap.h>

size_t hfn(const void *x)
{
	return (size_t)x;
}

bool efn(const void *x, const void *y)
{
	return x == y;
}

void* dfn(const void *x)
{
	return (void*)x;
}

const char* /*UNITTEST*/
test_basic(void)
{
	hmap_t mp = hmap_init(300, hfn, efn, dfn);
	if (hmap_count(mp) != 0)
		return "newly allocated map not empty";
	
	hmap_dispose(mp);

	return NULL;
}

const char* /*UNITTEST*/
test_insert(void)
{
	int a, b, c, d, e, f, g, h, j;
	int *k = &a, *l = &b, *m = &c, *n = &d;
	int *v = &e, *w = &f, *x = &g, *y = &h;
	int *t = &j;

	hmap_t mp = hmap_init(300, hfn, efn, dfn);
	hmap_put(mp, k, v);
	hmap_put(mp, l, w);
	hmap_put(mp, m, x);
	hmap_put(mp, n, y);

	if (hmap_count(mp) != 4)
		return "count not 4 after putting 4 elements";

	if (hmap_get(mp, k) != v
	    || hmap_get(mp, l) != w
	    || hmap_get(mp, m) != x
	    || hmap_get(mp, n) != y
	    || hmap_get(mp, t) != NULL)
		return "got something else out than what was put in";

	hmap_put(mp, k, t);

	if (hmap_count(mp) != 4)
		return "count changed when it wasn't supposed to";

	if (hmap_get(mp, k) != t)
		return "replacing put() failed";

	hmap_clear(mp);

	if (hmap_count(mp) != 0)
		return "map not empty after cleaning";

	hmap_dispose(mp);

	return NULL;
}

const char* /*UNITTEST*/
test_iter(void)
{
	int a, b, c, d, e, f, g, h;
	int *k = &a, *l = &b, *m = &c, *n = &d;
	int *v = &e, *w = &f, *x = &g, *y = &h;

	hmap_t mp = hmap_init(229, hfn, efn, dfn);
	hmap_put(mp, k, v);
	hmap_put(mp, l, w);
	hmap_put(mp, m, x);
	hmap_put(mp, n, y);

	if (hmap_count(mp) != 4)
		return "count not 4 after putting 4 elements";

	void *key, *val;

	if (!hmap_first(mp, &key, &val))
		return "hmap_first() failed";
	
	bool seena = false, seenb = false, seenc = false, seend = false;
	if (key == &a || val == &e)
		seena = true;
	if (key == &b || val == &f)
		seenb = true;
	if (key == &c || val == &g)
		seenc = true;
	if (key == &d || val == &h)
		seend = true;

	for(int i = 0; i < 3; i++) {
		if (!hmap_next(mp, &key, &val))
			return "hmap_next failed";

		if (key == &a || val == &e)
			seena = true;
		if (key == &b || val == &f)
			seenb = true;
		if (key == &c || val == &g)
			seenc = true;
		if (key == &d || val == &h)
			seend = true;
	}

	if (!seena || !seenb || !seenc || !seend)
		return "iterator fail";

	if (!hmap_next(mp, &key, &val))
		return "hmap_next failed";

	if (key || val)
		return "extra elements after iterator ended";

	if (hmap_next(mp, &key, &val))
		return "iterator still valid";

	hmap_dispose(mp);

	return NULL;

}
