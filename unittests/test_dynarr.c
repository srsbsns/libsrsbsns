/* test_dynarr.c - (C) 2014, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#include "unittests_common.h"

#include <libsrsbsns/dynarr.h>

#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

const char* /*UNITTEST*/
test_basic(void)
{
	dynarr_t a = dynarr_init(0);
	if (dynarr_count(a) != 0)
		return "newly allocated dynarr not empty";
	
	dynarr_dispose(a);

	return NULL;
}

const char* /*UNITTEST*/
test_access(void)
{
	int ia[] = {0,1,2,3,4};
	dynarr_t a = dynarr_init(1);

	dynarr_put(a, 0, &ia[0]);
	dynarr_push(a, &ia[1]);
	dynarr_push(a, &ia[2]);

	if (dynarr_count(a) != 3)
		return "array count wrong";

	if (*(int*)dynarr_get(a, 0) != 0)
		return "wrong element at index 0";
	if (*(int*)dynarr_get(a, 1) != 1)
		return "wrong element at index 1";
	if (*(int*)dynarr_get(a, 2) != 2)
		return "wrong element at index 2";

	dynarr_put(a, 2, &ia[3]);
	if (*(int*)dynarr_get(a, 2) != 3)
		return "wrong element at index 2 after put";

	dynarr_put(a, 125, &ia[4]);
	if (dynarr_count(a) != 126)
		return "array didn't grow properly";
	
	dynarr_get(a, 1337);
	if (dynarr_count(a) != 1338)
		return "array didn't grow properly";

	if (*(int*)dynarr_get(a, 125) != ia[4])
		return "wrong element at index 125 after growing put";

	dynarr_dispose(a);

	return NULL;
}
