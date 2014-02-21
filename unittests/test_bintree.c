/* test_bintree.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/bintree.h>

int
intcmp(void *x, void *y)
{
	return *(int*)x - *(int*)y;
}

const char* /*UNITTEST*/
test_basic(void)
{
	bintree_t t = bintree_init(intcmp);
	if (bintree_count(t) != 0)
		return "newly allocated tree not empty";
	
	bintree_dispose(t);

	return NULL;
}

const char* /*UNITTEST*/
test_find(void)
{
	bintree_t t = bintree_init(intcmp);
	const size_t N = 100;

	int *ia = malloc(N * sizeof *ia);
	int *ca = malloc(N * sizeof *ca);
	for (size_t i = 0; i < N; i++)
		if ((ia[i] = ca[i] = rand()) == 1337) { //don't put in 1337
			i--;
			continue;
		}
	
	for (size_t i = 0; i < N; i++)
		bintree_insert(t, &ia[i]);

	for (size_t i = 0; i < N; i++)
		if (!bintree_find(t, &ia[i]))
			return "didn't find element previously put in";

	int i = 1337;
	if (bintree_find(t, &i))
		return "found element /not/ put in";

	bintree_dispose(t);

	return NULL;
}
