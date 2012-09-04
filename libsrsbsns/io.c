/* io.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libsrsbsns/io.h>

#include <libsrslog/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include <unistd.h>

#define ISLINETERM(C) ((C) == '\n' || (C) == '\r' || (C) == '\0')
int
io_read_line(int fd, char *dest, size_t dest_sz)
{
	size_t bc = 0;
	char c = '\0';
	int r;
	do {
		r = read(fd, &c, 1);
		//fprintf(stderr, "read: %d (%hhx; %c)\n", r, (char)(r == 1 ? c : 0), (char)(r == 1 ? c : '?'));
		if      (r == 0)  break; //EOF
		else if (r == -1) return -1;
		assert (r == 1);

		if (bc < dest_sz) {
			dest[bc] = c;
			//fprintf(stderr, "dest[%d] = %#hhx\n", bc, c);
		}

		bc++;
	} while(!ISLINETERM(c));

	//fprintf(stderr, "end of loop, bc is %d%s\n", bc, c == '\0'?" (will be decremented)":"");

	if (c == '\0')
		bc--;

	//fprintf(stderr, "adding NUL at dest[%d]\n", (int)(bc >= dest_sz ? dest_sz - 1 : bc));
	dest[(bc >= dest_sz) ? dest_sz - 1 : bc] = '\0';
	//fprintf(stderr, "returning bc i.e. %d\n", bc);
	return bc;
}
#undef ISLINETERM
