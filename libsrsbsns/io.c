/* io.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <assert.h>

#include <unistd.h>

#include <err.h>

#include "intlog.h"

#include <libsrsbsns/misc.h>
#include <libsrsbsns/io.h>


#define ISLINETERM(C) ((C) == '\n' || (C) == '\r' || (C) == '\0')
#define MAX_WRITEBUF 4096


int
io_read_line(int fd, char *dest, size_t dest_sz)
{
	size_t bc = 0;
	char c = '\0';
	int r;
	do {
		errno = 0;
		r = read(fd, &c, 1);
		//fprintf(stderr, "read: %d (%hhx; %c)\n", r, (char)(r == 1 ? c : 0), (char)(r == 1 ? c : '?'));
		if (r == 0) {
			warnx("EOF after %zu/%zu bytes (and c is %hhx)", bc, dest_sz, c);
			break; //EOF
		}
		else if (r == -1) {
			warn("read failed after %zu/%zu bytes", bc, dest_sz);
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
			warn("io_writeall, write() failed");
			return false;
		}
		bc += (size_t)r;
	}
	return true;
}

int
io_select1w(int fd, int64_t to_us)
{
	return io_select(NULL, 0, &fd, 1, NULL, 0, to_us);
}

int
io_select1r(int fd, int64_t to_us)
{
	return io_select(&fd, 1, NULL, 0, NULL, 0, to_us);
}

int
io_select(int *rfd, size_t num_rfd,
           int *wfd, size_t num_wfd,
           int *efd, size_t num_efd,
	   int64_t to_us)
{
	int64_t trem = 0;
	int preverrno = errno;
	int64_t tsend = to_us ? tstamp_us() + to_us : 0;
	struct timeval tout;
	tout.tv_sec = 0;
	tout.tv_usec = 0;
	for(;;) {
		fd_set rfds, wfds, efds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		int maxfd = -1;

		for (size_t i = 0; i < num_rfd; i++) {
			FD_SET(rfd[i], &rfds);
			if (rfd[i] > maxfd)
				maxfd = rfd[i];
		}

		for (size_t i = 0; i < num_wfd; i++) {
			FD_SET(wfd[i], &wfds);
			if (wfd[i] > maxfd)
				maxfd = wfd[i];
		}

		for (size_t i = 0; i < num_efd; i++) {
			FD_SET(efd[i], &efds);
			if (efd[i] > maxfd)
				maxfd = efd[i];
		}

		if (maxfd < 0) {
			E("no fds given?");
			errno = EINVAL;
			return -1;
		}

		if (tsend) {
			trem = tsend - tstamp_us();

			if (trem <= 0) {
				D("timeout reached");
				return 0;
			}

			tconv(&tout, &trem, false);
		}

		int r = select(maxfd+1, &rfds, &wfds, &efds, tsend ? &tout : NULL);
		if (!r)
			continue;

		if (r < 0) {
			if (errno == EINTR) {
				WE("select");
				errno = preverrno;
				continue;
			}

			WE("select() failed");
		 } else if (r > 0) {
			D("selected!");
			for (size_t i = 0; i < num_rfd; i++)
				if (!FD_ISSET(rfd[i], &rfds))
					rfd[i] = -1;

			for (size_t i = 0; i < num_wfd; i++)
				if (!FD_ISSET(wfd[i], &wfds))
					wfd[i] = -1;

			for (size_t i = 0; i < num_efd; i++)
				if (!FD_ISSET(efd[i], &efds))
					efd[i] = -1;
		}

		return r;
	}
}
#undef ISLINETERM
