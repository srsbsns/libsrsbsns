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
#define MAX_WRITEBUF 4096


int
io_read_line(int fd, char *dest, size_t dest_sz)
{
	size_t bc = 0;
	char c = '\0';
	int r;
	do {
		r = read(fd, &c, 1);
		//fprintf(stderr, "read: %d (%hhx; %c)\n", r, (char)(r == 1 ? c : 0), (char)(r == 1 ? c : '?'));
		if (r == 0) {
			W("EOF after %zu/%zu bytes (and c is %hhx)", bc, dest_sz, c);
			break; //EOF
		}
		else if (r == -1) {
			W("read failed after %zu/%zu bytes", bc, dest_sz);
			return -1;
		}
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

int
io_fprintf(int fd, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	int r = io_vfprintf(fd, fmt, l);
	va_end(l);
	return r;
}

int
io_vfprintf(int fd, const char *fmt, va_list ap)
{
	char buf[MAX_WRITEBUF];
	int r = vsnprintf(buf, sizeof buf, fmt, ap);


	if (!io_writeall(fd, buf, strlen(buf)))
		return -1;
	
	return r;
}

bool
io_writeall(int fd, const char *buf, size_t n)
{
	size_t bc = 0;
	while(bc < n) {
		ssize_t r = write(fd, buf + bc, n - bc);
		if (r == -1) {
			WE("io_writeall, write() failed");
			return false;
		}
		bc += (size_t)r;
	}
	return true;
}
#undef ISLINETERM
