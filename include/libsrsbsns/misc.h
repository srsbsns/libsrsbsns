/* misc.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_MISC_H
#define LIBSRSBSNS_MISC_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

int64_t tstamp_us(void);
void tconv(struct timeval *tv, int64_t *ts, bool tv_to_ts);

bool isdigitstr(const char *p);

void strNcat(char *dest, const char *src, size_t destsz);
char* strNcpy(char *dst, const char *src, size_t len);

//call with dest_nelem = 0 to get number of args which would have been extracted
int splitquoted(char *input, char **dest, size_t dest_nelem);

#endif /* LIBSRSBSNS_MISC_H */
