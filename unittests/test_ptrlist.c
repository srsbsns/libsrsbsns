/* test_ptrlist.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/ptrlist.h>

const char* /*UNITTEST*/
test_basic(void)
{
	ptrlist_t l = ptrlist_init();
	if (ptrlist_count(l) != 0)
		return "newly allocated list not empty";
	
	ptrlist_dispose(l);

	return NULL;
}

const char* /*UNITTEST*/
test_insert(void)
{
	ptrlist_t l = ptrlist_init();
	
	int w, x, y, z;
	int *p = &w, *q = &x, *r = &y, *s = &z;

	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);
	ptrlist_insert(l, 0, q);
	ptrlist_insert(l, 0, p);

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != q
	    || ptrlist_get(l, 2) != r
	    || ptrlist_get(l, 3) != s
	    || ptrlist_get(l, 4) != NULL)
		return "got something else out than what was put in";

	ptrlist_clear(l);

	ptrlist_insert(l, -1, p);
	ptrlist_insert(l, -1, q);
	ptrlist_insert(l, -1, r);
	ptrlist_insert(l, -1, s);

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != q
	    || ptrlist_get(l, 2) != r
	    || ptrlist_get(l, 3) != s
	    || ptrlist_get(l, 4) != NULL)
		return "got something else out than what was put in";

	ptrlist_clear(l);

	ptrlist_insert(l, 5, r); //tail
	ptrlist_insert(l, -1, s); //tail
	ptrlist_insert(l, 0, p); //head
	ptrlist_insert(l, 1, q); //2nd

	if (ptrlist_count(l) != 4)
		return "list size != 4 after inserting four elements";

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != q
	    || ptrlist_get(l, 2) != r
	    || ptrlist_get(l, 3) != s
	    || ptrlist_get(l, 4) != NULL)
		return "got something else out than what was put in";

	ptrlist_dispose(l);

	return NULL;
}

const char* /*UNITTEST*/
test_remove(void)
{
	ptrlist_t l = ptrlist_init();
	
	int w, x, y, z;
	int *p = &w, *q = &x, *r = &y, *s = &z;

	ptrlist_insert(l, 0, s);

	ptrlist_remove(l, 1);

	if (ptrlist_count(l) != 1)
		return "bogus remove actually removed something?";

	ptrlist_remove(l, 0);

	if (ptrlist_count(l) != 0)
		return "failed to remove single element";
	

	ptrlist_clear(l);


	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);

	ptrlist_remove(l, 2);

	if (ptrlist_count(l) != 2)
		return "bogus remove actually removed something?";

	ptrlist_remove(l, 0);

	if (ptrlist_count(l) != 1)
		return "failed to remove head of 2-element list";

	ptrlist_remove(l, 0);

	if (ptrlist_count(l) != 0)
		return "failed to remove single remaining element";
	

	ptrlist_clear(l);

	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);
	ptrlist_insert(l, 0, q);
	ptrlist_insert(l, 0, p);

	ptrlist_remove(l, 0);

	if (ptrlist_get(l, 0) != q
	    || ptrlist_get(l, 1) != r
	    || ptrlist_get(l, 2) != s
	    || ptrlist_get(l, 3) != NULL)
		return "remove of element 0 in 4-elem list failed";

	ptrlist_clear(l);

	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);
	ptrlist_insert(l, 0, q);
	ptrlist_insert(l, 0, p);

	ptrlist_remove(l, 1);

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != r
	    || ptrlist_get(l, 2) != s
	    || ptrlist_get(l, 3) != NULL)
		return "remove of element 1 in 4-elem list failed";

	ptrlist_clear(l);

	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);
	ptrlist_insert(l, 0, q);
	ptrlist_insert(l, 0, p);

	ptrlist_remove(l, 2);

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != q
	    || ptrlist_get(l, 2) != s
	    || ptrlist_get(l, 3) != NULL)
		return "remove of element 2 in 4-elem list failed";

	ptrlist_clear(l);

	ptrlist_insert(l, 0, s);
	ptrlist_insert(l, 0, r);
	ptrlist_insert(l, 0, q);
	ptrlist_insert(l, 0, p);

	ptrlist_remove(l, 3);

	if (ptrlist_get(l, 0) != p
	    || ptrlist_get(l, 1) != q
	    || ptrlist_get(l, 2) != r
	    || ptrlist_get(l, 3) != NULL)
		return "remove of element 3 in 4-elem list failed";

	ptrlist_clear(l);

	ptrlist_dispose(l);

	return NULL;
}
