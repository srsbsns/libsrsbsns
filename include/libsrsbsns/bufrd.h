/* bufrd.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_BUFRD_H
#define LIBSRSBSNS_BUFRD_H 1

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct bufrd *bufrd_t;

bufrd_t bufrd_init(int fd, size_t bufsz);
void bufrd_dispose(bufrd_t b);

ssize_t bufrd_read(bufrd_t b, void *dest, size_t nbytes);
int bufrd_getchar(bufrd_t b);
bool bufrd_ungetchar(bufrd_t b, int c);
bool bufrd_eof(bufrd_t b);
bool bufrd_buffered(bufrd_t b);

#endif /* LIBSRSBSNS_BUFRD_H */
