/* test_bintree.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include <time.h>

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
check_iter_vs_fixed(char *inp, char *chk, size_t len)
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

static const char*
check_iter_vs_rec(char *inp, size_t len)
{
	const char *e = NULL;
	bintree_t t = bintree_init(chcmp);

	for (size_t i = 0; i < len; i++)
		if (!bintree_insert(t, &inp[i]))
			return "insertion failed";


	void **rec = malloc((len+1) * sizeof *rec);
	char nc = 0;
	for (size_t i = 0; i < len; i++) {
		rec[i] = &nc;
	}

	
	if (!bintree_collect(t, rec, TRAV_PREORDER)) {
		e = "bintree_collect failed";
		goto out;
	}

	//fprintf(stderr, "rec: '");
	//for (size_t i = 0; i < len; i++) {
		//if (rec[i])
			//fprintf(stderr, "%c", *(char*)rec[i]);
		//else break;
	//}
	//fprintf(stderr, "\n");

	//fprintf(stderr, "ite: '");
	char *ch;
	size_t bc = bintree_count(t);
	for (size_t i = 0; i < bc; i++) {
		if (!i) {
			if (!(ch = bintree_first(t, TRAV_PREORDER))) {
				e = "bintree_first failed";
				goto out;
			}
		} else {
			if (!(ch = bintree_next(t))) {
				e = "bintree_next failed";
				goto out;
			}
		}
		//fprintf(stderr, "%c", *ch);

		if (*ch != *(char*)rec[i]) {
			e = "iterative and recursive traversal disagree";
			goto out;
		}
	}
	//fprintf(stderr, "\n");

	if (bintree_next(t)) {
		e = "bintree_next did not fail when it should";
		goto out;
	}

out:
	free(rec);
	bintree_dispose(t);
	return e;
	
}

const char* /*UNITTEST*/
test_iter(void)
{
	char data[20];
	time_t seed = time(NULL);
	srand(seed);
	for (size_t i = 0; i < 100000; i++) {
		size_t len = rand() % (sizeof data - 2) + 1;
		//fprintf(stderr, "i: %zu, len: %zu", i, len);
		for (size_t j = 0; j < len; j++)
			data[j] = 'A' + (rand()>>3) % 26;
		data[len] = '\0';
		//fprintf(stderr, ", tree: '%s'\n", data);
		const char *e = check_iter_vs_rec(data, len);
		if (e) {
			fprintf(stderr, "seed was: %lu\n", seed);
			return e;
		}

	}
       const char *s, *c;
       char chk[sizeof data];
       const char *e;

       //check some fixed
       s = "FBADCEGIH"; c = "FBADCEGIH"; strcpy(data, c); strcpy(chk, s); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "A"; c = "A"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "AB"; c = "AB"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "BA"; c = "BA"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "BAC"; c = "BAC"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "BCA"; c = "BAC"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;
       s = "MFZEIGKH"; c = "MFEIGHKZ"; strcpy(data, s); strcpy(chk, c); e = check_iter_vs_fixed(data, chk, strlen(data)); if (e) return e;


	return NULL;

}
