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

int
chcmp(void *x, void *y)
{
	return *(char*)x - *(char*)y;
}

const char* /*UNITTEST*/
test_basic(void)
{
	bintree_t t = bintree_init(intcmp);
	if (!t)
		return "couldn't init bintree";

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
	for (size_t i = 0; i < N; i++)
		if ((ia[i] = rand()) == 1337) { //don't put in 1337
			i--;
			continue;
		}
	
	for (size_t i = 0; i < N; i++)
		if (!bintree_insert(t, &ia[i]))
			return "insertion failed";

	for (size_t i = 0; i < N; i++)
		if (!bintree_find(t, &ia[i]))
			return "didn't find element previously put in";

	int i = 1337;
	if (bintree_find(t, &i))
		return "found element /not/ put in";

	bintree_dispose(t);

	return NULL;
}

const char* /*UNITTEST*/
test_iter(void)
{
	bintree_t t = bintree_init(chcmp);

	char da[] = "FBADCEGIH";
	char res[sizeof da];

	for (size_t i = 0; i < strlen(da); i++)
		if (!bintree_insert(t, &da[i]))
			return "insertion failed";


	char *ch;
	for (size_t i = 0; i < strlen(da); i++) {
		if (!i) {
			if (!(ch = bintree_first(t, TRAV_PREORDER)))
				return "bintree_first failed";
		} else {
			if (!(ch = bintree_next(t)))
				return "bintree_next failed";
		}

		res[i] = *ch;
	}

	
	for (size_t i = 0; i < strlen(da); i++) {
		if (res[i] != "FBADCEGIH"[i])
			return "traversal failed";
	}

	if (bintree_next(t))
		return "bintree_next did not fail when it should";
		
	bintree_dispose(t);

	return NULL;

}
