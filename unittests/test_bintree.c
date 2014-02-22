/* test_bintree.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include <time.h>
#include <math.h>

#include "unittests_common.h"

#include <libsrsbsns/bintree.h>

void
chdumpfn(void *i)
{
	fprintf(stderr, "%c", *(char*)i);
}

void
dumpfn(void *i)
{
	fprintf(stderr, "%d", *(int*)i);
}

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
test_insert(void)
{
	int a = 3, *p = &a;
	int b = 2, *q = &b;

	bintree_t t = bintree_init(intcmp);

	if (!bintree_insert(t, q))
		return "insertion failed";

	if (bintree_count(t) != 1)
		return "wrong count after insert";

	if (!bintree_insert(t, q))
		return "insertion (again) failed";

	if (bintree_count(t) != 1)
		return "wrong count after same insert again";

	if (!bintree_insert(t, p))
		return "insertion failed 3";

	if (bintree_count(t) != 2)
		return "wrong count after another insert";

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
check_balance(char *inp, size_t len)
{
	const char *e = NULL;
	bintree_t t = bintree_init(chcmp);

	for (size_t i = 0; i < len; i++)
		if (!bintree_insert(t, &inp[i]))
			return "insertion failed";

	void **rec = malloc((len+1) * sizeof *rec);
	void **chk = malloc((len+1) * sizeof *chk);
	char nc = 0;
	for (size_t i = 0; i < len; i++) {
		rec[i] = &nc;
		chk[i] = &nc;
	}


	if (!bintree_collect(t, rec, TRAV_INORDER)) {
		e = "bintree_collect failed";
		goto out;
	}

	size_t height = bintree_height(t);
	//fprintf(stderr, "height is %zu\n", height);
	//bintree_dump(t, chdumpfn);

	bintree_balance(t);

	size_t nheight = bintree_height(t);
	//fprintf(stderr, "nheight is %zu\n", nheight);

	if (1 + (int)log2(bintree_count(t)) < nheight) {
		e = "bad height after balance";
		goto out;
	}

	//bintree_dump(t, chdumpfn);

	if (!bintree_collect(t, chk, TRAV_INORDER)) {
		e = "bintree_collect failed (2)";
		goto out;
	}

	size_t bc = bintree_count(t);
	for (size_t i = 0; i < bc; i++) {
		if (rec[i] != chk[i]) {
			e = "tree messed up after balance";
			goto out;
		}
	}

out:
	free(rec);
	free(chk);
	bintree_dispose(t);
	return e;
	
}
static const char*
check_iter_vs_rec(char *inp, size_t len, int travtype)
{
	const char *e = NULL;
	bintree_t t = bintree_init(chcmp);

	for (size_t i = 0; i < len; i++)
		if (!bintree_insert(t, &inp[i]))
			return "insertion failed";


	//bintree_dump(t, chdumpfn);
	void **rec = malloc((len+1) * sizeof *rec);
	char nc = 0;
	for (size_t i = 0; i < len; i++) {
		rec[i] = &nc;
	}


	if (!bintree_collect(t, rec, travtype)) {
		e = "bintree_collect failed";
		goto out;
	}

	/*fprintf(stderr, "rec: '");
	for (size_t i = 0; i < len; i++) {
		if (rec[i])
			fprintf(stderr, "%c", *(char*)rec[i]);
		else break;
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "ite: '");*/
	char *ch;
	size_t bc = bintree_count(t);
	for (size_t i = 0; i < bc; i++) {
		if (!i) {
			if (!(ch = bintree_first(t, travtype))) {
				e = "bintree_first failed";
				//fprintf(stderr, "\n");
				goto out;
			}
		} else {
			if (!(ch = bintree_next(t))) {
				e = "bintree_next failed";
				//fprintf(stderr, "\n");
				goto out;
			}
		}
		//fprintf(stderr, "%c", *ch);

		if (*ch != *(char*)rec[i]) {
			e = "iterative and recursive traversal disagree";
			//fprintf(stderr, "\n");
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
	int tt[] = {TRAV_PREORDER, TRAV_INORDER, TRAV_POSTORDER};

	for (size_t ti = 0; ti < sizeof tt / sizeof tt[0]; ti++)
		for (size_t i = 0; i < 100000; i++) {
			size_t len = rand() % (sizeof data - 2) + 1;
			//fprintf(stderr, "i: %zu, len: %zu", i, len);
			for (size_t j = 0; j < len; j++)
				data[j] = 'A' + (rand()>>3) % 26;
			data[len] = '\0';
			//fprintf(stderr, "inp: '%s'\n", data);
			const char *e = check_iter_vs_rec(data, len, tt[ti]);
			if (e) {
				fprintf(stderr, "seed was: %lu\n", seed);
				return e;
			}

		}

	return NULL;

}

const char* /*UNITTEST*/
test_balance(void)
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
		//fprintf(stderr, "inp: '%s'\n", data);
		const char *e = check_balance(data, len);
		if (e) {
			fprintf(stderr, "seed was: %lu\n", seed);
			return e;
		}
	}


	return NULL;

}


const char*
check_remove_1(void)
{
	bintree_t t = bintree_init(intcmp);

	int one = 1, two = 2, three = 3;

	if (!bintree_insert(t, &one))
		return "insertion failed";
	if (!bintree_insert(t, &two))
		return "insertion failed";
	if (!bintree_insert(t, &three))
		return "insertion failed";

	if (bintree_count(t) != 3)
		return "wrong count after insert";

	void *arr[3];
	if (!bintree_collect(t, arr, TRAV_PREORDER))
		return "collect failed (1)";

	if (*(int*)arr[0] != 1 || *(int*)arr[1] != 2 || *(int*)arr[2] != 3) {
		return "remove messed up the tree";
	}

	if (!bintree_remove(t, &one))
		return "remove failed";


	if (bintree_count(t) != 2)
		return "wrong count after remove";
	
	if (!bintree_collect(t, arr, TRAV_PREORDER))
		return "collect failed (2)";

	if (*(int*)arr[0] != 2 || *(int*)arr[1] != 3) {
		return "remove messed up the tree (2)";
	}

	if (bintree_remove(t, &one))
		return "remove did not fail";


	if (!bintree_remove(t, &three))
		return "remove failed";


	if (bintree_count(t) != 1)
		return "wrong count after remove";
	
	if (!bintree_collect(t, arr, TRAV_PREORDER))
		return "collect failed (3)";

	if (*(int*)arr[0] != 2)
		return "remove messed up the tree (3)";


	if (bintree_remove(t, &three))
		return "remove did not fail (2)";

	if (!bintree_remove(t, &two))
		return "remove failed";

	if (bintree_count(t) != 0)
		return "wrong count after remove";
	
	if (!bintree_collect(t, arr, TRAV_PREORDER))
		return "collect failed (4)";

	bintree_dispose(t);

	return NULL;
}




const char* /*UNITTEST*/
test_remove(void)
{
	const char *e = check_remove_1();
	if (e)
		return e;

	//e = check_remove_2();
	//if (e)
		//return e;

	return NULL;
}

