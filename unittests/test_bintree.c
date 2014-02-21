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

static const char*
check_preorder(char *inp, char *chk, size_t len)
{
	bintree_t t = bintree_init(chcmp);

	for (size_t i = 0; i < len; i++)
		if (!bintree_insert(t, &inp[i]))
			return "insertion failed";


	char *res = malloc(len+1);
	res[len] = '\0';

	char *ch;
	for (size_t i = 0; i < len; i++) {
		if (!i) {
			if (!(ch = bintree_first(t, TRAV_PREORDER))) {
				free(res);
				return "bintree_first failed";
			}
		} else {
			if (!(ch = bintree_next(t))) {
				return "bintree_next failed";
				free(res);
			}
		}

		res[i] = *ch;
	}
	if (bintree_next(t))
		return "bintree_next did not fail when it should";


	if (strcmp(res, chk) != 0) {
		printf("got: '%s', want: '%s'\n", res, chk);
		free(res);
		return "traversal failure";
	}

	free(res);
	bintree_dispose(t);
	return NULL;

}

const char* /*UNITTEST*/
test_iter(void)
{
	char data[1024];
	char chk[1024];

	const char *s;
	const char *c;
	const char *e;
	s = "FBADCEGIH"; c = "FBADCEGIH"; strcpy(data, c); strcpy(chk, s); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "A"; c = "A"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "AB"; c = "AB"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "BA"; c = "BA"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "BAC"; c = "BAC"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "BCA"; c = "BAC"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	s = "MFZEIGKH"; c = "MFEIGHKZ"; strcpy(data, s); strcpy(chk, c); e = check_preorder(data, chk, strlen(data)); if (e) return e;
	return NULL;

}
/*

              M
      F              Z
E         I
       G     K
	H
*/
