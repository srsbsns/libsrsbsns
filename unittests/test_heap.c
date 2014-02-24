/* test_heap.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/heap.h>


#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

int
icmp(void *e1, void *e2)
{
	return *(int*)e1 - *(int*)e2;
}

void
idump(void *e)
{
	fprintf(stderr, "%d", *(int*)e);
}

const char* /*UNITTEST*/
test_basic(void)
{
	heap_t mp = heap_init(icmp);
	if (heap_count(mp) != 0)
		return "newly allocated map not empty";
	
	heap_dispose(mp);

	return NULL;
}

const char* /*UNITTEST*/
test_insert(void)
{
	heap_t mp = heap_init(icmp);
	int ia[100];
	for (int i = 0; i < COUNTOF(ia); i++) {
		ia[i] = rand();
		heap_insert(mp, &ia[i]);
	}
	heap_dump(mp, idump);
	
	heap_dispose(mp);

	return NULL;
}
