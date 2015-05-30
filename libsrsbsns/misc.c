/* misc.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <libsrsbsns/misc.h>

#include "intlog.h"

int64_t
tstamp_us(void)
{
	struct timeval t;
	int64_t ts = 0;
	if (gettimeofday(&t, NULL) != 0)
		E("gettimeofday");
	else
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
		if (!isdigit(*p++))
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

int
splitquoted(char *input, char **dest, size_t dest_nelem)
{
	if (dest_nelem > INT_MAX) {
		E("dest_nelem too large (%zu > %d)", dest_nelem, INT_MAX);
		return -1;
	}

	char quotetype = 0;
	bool inws = true;
	size_t n = 0;

	while (*input) {
		switch (*input) {
		case '\'':
		case '"':
			if (inws) {
				quotetype = *input;
				inws = false;
				if (n < dest_nelem)
					dest[n] = input + 1; //skip the quote
				n++;
			} else if (quotetype == *input) {
				quotetype = 0;
				inws = true;
				*input = '\0'; //drop the quote
			} else if (!quotetype) {
				//quote in unquoted word starts
				//a new token (not true for sh)
				quotetype = *input;
				*input = '\0';
				if (n < dest_nelem)
					dest[n] = input + 1; //skip the quote
				n++;
			}
			break;

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (inws)
				break;
			
			if (!quotetype) {
				inws = true;
				*input = '\0';
			}

			break;
		default:
			if (inws) {
				quotetype = 0;
				inws = false;
				if (n < dest_nelem)
					dest[n] = input;
				n++;
			}
		}

		input++;
	}

	return n;
}
