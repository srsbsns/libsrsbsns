/* bufrd.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libsrsbsns/ringbuf.h>
#include <libsrsbsns/bufrd.h>

#include <stdlib.h>
#include <string.h>

#include <err.h>

struct bufrd {
	int fd;
	ringbuf_t buf;
	int unget;
	bool eof;
};

static ssize_t read_more(bufrd_t b);


bufrd_t
bufrd_init(int fd, size_t bufsz)
{
	bufrd_t b = malloc(sizeof (*(bufrd_t)0));

	if (!b)
		return NULL;

	b->fd = fd;
	b->buf = ringbuf_init(bufsz);
	b->unget = -1;
	b->eof = false;

	return b;
}

void
bufrd_dispose(bufrd_t b)
{
	ringbuf_dispose(b->buf);
	free(b);
}

// return 0 on eof, -1 on error, number read otherwise
// zero length buffer is an error
ssize_t
bufrd_read(bufrd_t b, void *dest, size_t nbytes)
{
	if (!nbytes)
		return -1;

	if (b->unget == -1 && ringbuf_empty(b->buf)) {
		ssize_t r = read_more(b);
		if (r <= 0)
			return r;
	}
	
	bool ungot = false;
	if (b->unget != -1) {
		((uint8_t *)dest)[0] = (uint8_t)b->unget;
		nbytes--;
		dest=((uint8_t *)dest) + 1;
		b->unget = -1;
		ungot = true;
	}

	if (!ringbuf_empty(b->buf))
		return ungot + (size_t)ringbuf_get(b->buf, dest, nbytes);
	else
		return ungot;
}

int
bufrd_getchar(bufrd_t b)
{
	uint8_t ch;
	if (b->unget != -1) {
		ch = (uint8_t)b->unget;
		b->unget = -1;
		return ch;
	}

	ssize_t r = bufrd_read(b, &ch, 1);

	if (r <= 0) {
		b->eof = r == 0;
		return -1;
	}
	
	return ch;
}

bool
bufrd_eof(bufrd_t b)
{
	return b->eof;
}

bool
bufrd_buffered(bufrd_t b)
{
	return b->unget != -1 || !ringbuf_empty(b->buf);
}

bool
bufrd_ungetchar(bufrd_t b, int c)
{
	if (b->unget != -1)
		return false;
	
	b->unget = c;
	return true;
}

static ssize_t
read_more(bufrd_t b)
{
	uint8_t buf[4096];

	ssize_t r = read(b->fd, buf, sizeof buf);
	if (r > 0)	
		ringbuf_put(b->buf, buf, r);
	return r;
}
