/* test_stack.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/stack.h>

#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

void idump(void *e)
{
	fprintf(stderr, "``%d''", *(int*)e);
}

const char* /*UNITTEST*/
test_basic(void)
{
	stack_t s = stack_init(10);
	if (stack_count(s) != 0)
		return "newly allocated stack not empty";
	
	stack_dispose(s);

	return NULL;
}

const char* /*UNITTEST*/
test_pushpop(void)
{
	stack_t s = stack_init(1);
	const size_t N = 1000;
	int *ia = malloc(N * sizeof *ia);
	for (size_t i = 0; i < N; i++)
		stack_push(s, ((ia[i] = (rand()>>3)%100000), &ia[i]));

	if (stack_count(s) != N)
		return "wrong count after pushing";

	for (size_t i = 0; i < N; i++)
		if (*(int*)stack_pop(s) != ia[N-i-1])
			return "wrong value popped out of stack";

	return NULL;
}
	

const char* /*UNITTEST*/
test_iter(void)
{
	stack_t s = stack_init(1);
	const size_t N = 1000;
	int *ia = malloc(N * sizeof *ia);
	for (size_t i = 0; i < N; i++)
		stack_push(s, ((ia[i] = (rand()>>3)%100000), &ia[i]));

	for (size_t i = 0; i < N; i++)
		if (*(int*)(i?stack_next(s, true):stack_first(s, true)) != ia[i])
			return "wrong value seen while iterating";

	if (stack_next(s, true))
		return "stack_next did not return when it should";

	for (size_t i = 0; i < N; i++)
		if (*(int*)(i?stack_next(s, false):stack_first(s, false)) != ia[N-i-1])
			return "wrong value seen while iterating (2)";
	
	if (stack_next(s, false))
		return "stack_next did not return when it should (2)";

	for (size_t i = 0; i < N; i++)
		if (*(int*)(i?stack_next(s, true):stack_first(s, true)) != ia[i])
			return "wrong value seen while iterating (3)";
	for (size_t i = 1; i < N; i++)
		if (*(int*)(stack_next(s, false)) != ia[N-i-1])
			return "wrong value seen while iterating (4)";
	if (stack_next(s, false))
		return "stack_next did not return when it should (3)";
	

	return NULL;
}
