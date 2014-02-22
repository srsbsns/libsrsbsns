/* test_deque.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/deque.h>

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
	return "unit test not implemented";
}

const char* /*UNITTEST*/
test_grow(void)
{
	return "unit test not implemented";
}

const char* /*UNITTEST*/
test_iter(void)
{
	return "unit test not implemented";
}
