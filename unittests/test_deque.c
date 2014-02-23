/* test_deque.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/deque.h>

#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

void idump(void *e)
{
	fprintf(stderr, "``%d''", *(int*)e);
}

const char* /*UNITTEST*/
test_basic(void)
{
	deque_t l = deque_init(10);
	if (deque_count(l) != 0)
		return "newly allocated deque not empty";
	
	deque_dispose(l);

	return NULL;
}

const char* /*UNITTEST*/
test_pushpop(void)
{
	deque_t d = deque_init(20);
	deque_dump(d, idump);
	int ia[] = {0,1,2,3,4,5,6,7,8,9};
	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushback(d, &ia[i]))
			return "pushback failed";
	deque_dump(d, idump);
	
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushback";

	int *e;
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popback(d)))
			return "popback failed";

		if (*e != ia[9-i])
			return "wrong element popped out the back";
	}
	deque_dump(d, idump);

	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not";

	if (deque_popback(d))
		return "popback didn't fail when it should (1)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (1)";
	deque_dump(d, idump);

	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushback(d, &ia[i]))
			return "pushback failed (2)";
	

	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushback (2)";

	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popfront(d)))
			return "popfront failed";

		if (*e != ia[i])
			return "wrong element popped out the front";
	}

	deque_dump(d, idump);
	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (2)";

	if (deque_popback(d))
		return "popback didn't fail when it should (2)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (2)";
	deque_dump(d, idump);

	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushfront(d, &ia[i]))
			return "pushfront failed";
	
	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushfront";

	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popback(d)))
			return "popback failed (2)";

		if (*e != ia[i])
			return "wrong element popped out the back (2)";
	}

	deque_dump(d, idump);
	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (3)";

	deque_dump(d, idump);
	if (deque_popback(d))
		return "popback didn't fail when it should (3)";
	deque_dump(d, idump);

	if (deque_popfront(d))
		return "popfront didn't fail when it should (3)";

	fprintf(stderr, "======\n");
	deque_dump(d, idump);
	fprintf(stderr, "======\n");
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!deque_pushfront(d, &ia[i]))
			return "pushfront failed (2)";
		deque_dump(d, idump);
	}
	
	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushfront (2)";

	deque_dump(d, idump);
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popfront(d)))
			return "popfront failed (2)";

		if (*e != ia[9-i])
			return "wrong element popped out the front (2)";
	}
	deque_dump(d, idump);

	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (4)";

	if (deque_popback(d))
		return "popback didn't fail when it should (4)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (4)";

	deque_dump(d, idump);
	deque_dispose(d);

	return NULL;
}

const char* /*UNITTEST*/
test_grow(void)
{
	deque_t d = deque_init(0);
	deque_dump(d, idump);
	int ia[] = {0,1,2,3,4,5,6,7,8,9};
	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushback(d, &ia[i]))
			return "pushback failed";
	deque_dump(d, idump);
	
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushback";

	int *e;
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popback(d)))
			return "popback failed";

		if (*e != ia[9-i])
			return "wrong element popped out the back";
	}
	deque_dump(d, idump);

	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not";

	if (deque_popback(d))
		return "popback didn't fail when it should (1)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (1)";
	deque_dump(d, idump);

	deque_dispose(d);
	d = deque_init(0);
	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushback(d, &ia[i]))
			return "pushback failed (2)";
	
	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushback (2)";

	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popfront(d)))
			return "popfront failed";

		if (*e != ia[i])
			return "wrong element popped out the front";
	}

	deque_dump(d, idump);
	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (2)";

	if (deque_popback(d))
		return "popback didn't fail when it should (2)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (2)";
	deque_dump(d, idump);

	deque_dispose(d);
	d = deque_init(0);
	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushfront(d, &ia[i]))
			return "pushfront failed";
	
	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushfront";

	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popback(d)))
			return "popback failed (2)";

		if (*e != ia[i])
			return "wrong element popped out the back (2)";
	}

	deque_dump(d, idump);
	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (3)";

	deque_dump(d, idump);
	if (deque_popback(d))
		return "popback didn't fail when it should (3)";
	deque_dump(d, idump);

	if (deque_popfront(d))
		return "popfront didn't fail when it should (3)";

	deque_dispose(d);
	d = deque_init(0);
	for (size_t i = 0; i < COUNTOF(ia); i++)
		if (!deque_pushfront(d, &ia[i]))
			return "pushfront failed (2)";
	
	deque_dump(d, idump);
	if (deque_count(d) != COUNTOF(ia))
		return "wrong count after pushfront (2)";

	deque_dump(d, idump);
	for (size_t i = 0; i < COUNTOF(ia); i++) {
		if (!(e = deque_popfront(d)))
			return "popfront failed (2)";

		if (*e != ia[9-i])
			return "wrong element popped out the front (2)";
	}
	deque_dump(d, idump);

	if (deque_count(d) != 0)
		return "deque supposed to be empty but it's not (4)";

	if (deque_popback(d))
		return "popback didn't fail when it should (4)";
	deque_dump(d, idump);
	if (deque_popfront(d))
		return "popfront didn't fail when it should (4)";

	deque_dump(d, idump);
	deque_dispose(d);

	return NULL;
}

const char* /*UNITTEST*/
test_iter(void)
{
	return "unit test not implemented";
}
