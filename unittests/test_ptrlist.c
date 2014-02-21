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
	
	int x, *p = &x;
	ptrlist_insert(l, 0, p);

	if (ptrlist_count(l) != 1)
		return "list size != 1 after inserting one element";
	
	int *r = ptrlist_get(l, 0);
	if (r != p)
		return "got something else out than what was put in";

	ptrlist_dispose(l);

	return NULL;
}
