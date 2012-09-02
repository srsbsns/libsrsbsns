/* common.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h>

#define MAX_NICKLEN 64
#define MAX_UNAMELEN 64
#define MAX_FNAMELEN 128
#define MAX_PFXLEN 256

#define XCALLOC(num) xcalloc(1, num)
#define XMALLOC(num) xmalloc(num)
#define XREALLOC(p, num) xrealloc((p),(num))
#define XFREE(p) do{  if(p) free(p); p=0;  }while(0)
#define XSTRDUP(s) xstrdup(s)

#define ENSURE(FUNC, RET) \
                  xensure(FUNC, (RET), #FUNC, __FILE__, __LINE__, __func__)

void xensure(int ret, int exp, const char *fn, const char *file, int line,
		const char *caller);

void randstr(char *p, size_t numchars);
void strNcat(char *dest, const char *src, size_t destsz);
char* strNcpy(char *dst, const char *src, size_t len);

void *xcalloc(size_t num, size_t size);
void *xmalloc(size_t num);
void *xrealloc(void *p, size_t num);
char *xstrdup(const char *string);

int64_t timestamp_us();
void tconv(struct timeval *tv, int64_t *ts, bool tv_to_ts);

#endif /* COMMON_COMMON_H */
