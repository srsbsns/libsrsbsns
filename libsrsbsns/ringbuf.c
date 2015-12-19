/* ringbuf.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libsrsbsns/ringbuf.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <err.h>

struct ringbuf {
	unsigned char *buf;
	unsigned char *head;
	unsigned char *tail;
	size_t num;
	size_t bufsz;
};


ringbuf_t
ringbuf_init(size_t initsize)
{
	ringbuf_t b = malloc(sizeof (*(ringbuf_t)0));

	if (!b)
		return NULL;

	b->buf = malloc(initsize);
	b->bufsz = initsize;
	b->head = b->tail = b->buf;
	b->num = 0;

	return b;
}

void
ringbuf_dispose(ringbuf_t b)
{
	free(b->buf);
	free(b);
}

bool
ringbuf_empty(ringbuf_t b)
{
	return b->num == 0;
}

void
ringbuf_clear(ringbuf_t b)
{
	b->head = b->tail = b->buf;
	b->num = 0;
}

size_t
ringbuf_count(ringbuf_t b)
{
	return b->num;
}

static size_t
do_get(ringbuf_t b, unsigned char *data, size_t num, bool peek)
{
	if (b->num == 0)
		return 0;

	if (num > b->num)
		num = b->num;

	bool wrapped = b->num > 0 && b->head <= b->tail;
	size_t avail1 = !wrapped ? (size_t)(b->head - b->tail)
	                         : (b->bufsz - (b->tail - b->buf));

	if (avail1 >= num) {
		memcpy(data, b->tail, num);
		if (!peek)
			b->tail += num;
	} else {
		memcpy(data, b->tail, avail1);
		memcpy(data + avail1, b->buf, num - avail1);
		if (!peek)
			b->tail = b->buf + (num - avail1);
	}

	if (!peek)
		b->num -= num;

	return num;
}

size_t
ringbuf_get(ringbuf_t b, unsigned char *data, size_t num)
{
	return do_get(b, data, num, false);
}

size_t
ringbuf_peek(ringbuf_t b, unsigned char *data, size_t num)
{
	return do_get(b, data, num, true);
}

void
ringbuf_put(ringbuf_t b, unsigned char *data, size_t num)
{
	bool wrapped = b->num > 0 && b->head <= b->tail;
	size_t avail = b->bufsz - b->num;
	if (num > avail)
	{
		size_t nbufsz = b->bufsz;
		while(nbufsz - b->num < num)
			nbufsz *= 2;

		unsigned char *nbuf = malloc(nbufsz);
		if (!wrapped) {
			memcpy(nbuf, b->tail, b->head - b->tail);
		} else {
			size_t firstlen = b->bufsz - (b->tail - b->buf);
			memcpy(nbuf, b->tail, firstlen);
			memcpy(nbuf + firstlen, b->buf, b->head - b->buf);
		}
		b->head = nbuf + (b->bufsz - avail);
		b->tail = nbuf;
		free(b->buf);
		b->buf = nbuf;
		b->bufsz = nbufsz;
		wrapped = false;
		avail = nbufsz - b->num;
	}

	size_t avail1 = wrapped ? (size_t)(b->tail - b->head)
	                        : (b->bufsz - (b->head - b->buf));

	if (avail1 >= num) {
		memcpy(b->head, data, num);
		b->head += num;
	} else {
		memcpy(b->head, data, avail1);
		memcpy(b->buf, data + avail1, num - avail1);
		b->head = b->buf + (num - avail1);
	}
	b->num += num;
}


void
ringbuf_dump(ringbuf_t b)
{
	struct ringbuf *bf = b; // XXX
	fprintf(stderr, "ringbuf dump (hnd: %p)\n", b);
	fprintf(stderr, "distance start-head: %td\n", bf->head - bf->buf);
	fprintf(stderr, "distance head-tail: %td\n", bf->tail - bf->head);
	fprintf(stderr, "distance tail-end: %td\n", (ptrdiff_t)bf->bufsz - (bf->tail - bf->buf));
	fprintf(stderr, "bufsz: %zu, numelem :%zu\n", bf->bufsz, bf->num);
	fprintf(stderr, "actual buffer:");

	unsigned char *bufend = b->buf + b->bufsz;
	unsigned char *t = b->tail;
	for(size_t i = 0; i < bf->num; ++i)
	{
		fprintf(stderr, " %02x", *t);
		t++;
		if (t == bufend)
			t = b->buf;
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "sanitized string:");

	t = b->tail;
	for(size_t i = 0; i < bf->num; ++i)
	{
		if (isprint(*t))
			fprintf(stderr, "%c", *t);
		else
			fprintf(stderr, "<%02x>", *t);
		t++;
		if (t == bufend)
			t = b->buf;
	}
	fprintf(stderr, "\n");
}

