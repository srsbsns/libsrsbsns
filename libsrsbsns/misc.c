/* misc.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <ctype.h>

#include <libsrsbsns/misc.h>

int64_t
tstamp_us(void)
{
	struct timeval t;
	int64_t ts;
	gettimeofday(&t, NULL);
	tconv(&t, &ts, true);
	return ts;
}

void
tconv(struct timeval *tv, int64_t *ts, bool tv_to_ts)
{
	if (tv_to_ts)
		*ts = (int64_t)tv->tv_sec * 1000000 + tv->tv_usec;
	else {
		tv->tv_sec = *ts / 1000000;
		tv->tv_usec = *ts % 1000000;
	}
}

bool
isdigitstr(const char *p)
{
	while (*p)
		if (!isdigit(*p))
			return false;

	return true;
}

void
strNcat(char *dest, const char *src, size_t destsz)
{
	size_t len = strlen(dest);
	if (len + 1 >= destsz)
		return;

	size_t rem = destsz - (len + 1);

	char *ptr = dest + len;
	while(rem-- && *src)
		*ptr++ = *src++;

	*ptr = '\0';
}

char*
strNcpy(char *dst, const char *src, size_t len)
{
	char *r = strncpy(dst, src, len);
	dst[len-1] = '\0';
	return r;
}
