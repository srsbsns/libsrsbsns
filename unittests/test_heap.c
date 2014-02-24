/* test_heap.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/heap.h>


#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

int
icmpc(const void *e1, const void *e2)
{
	return *(const int*)e1 - *(const int*)e2;
}

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
	int ia[10];
	int ib[COUNTOF(ia)];
	int ic[COUNTOF(ia)];
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		ia[i] = ib[i] = ((rand()>>3)%200);
		heap_insert(mp, &ia[i]);
	}
	heap_dump(mp, idump);
	int *x;
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(x = heap_remove(mp)))
			return "remove failed";
		ic[i] = *x;
		fprintf(stderr, "pop: %d\n", ic[i]);
	}

	if ((x = heap_remove(mp)))
		return "remove did not fail when it should";

	qsort(ib, COUNTOF(ib), sizeof ib[0], icmpc);
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (ib[i] != ic[i])
			return "wrong result from removing";
	}
	
	heap_dispose(mp);

	return NULL;
}
