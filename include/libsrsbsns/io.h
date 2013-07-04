/* io.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_IO_H
#define LIBSRSBSNS_IO_H

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

int io_read_line(int fd, char *dest, size_t dest_sz);
int io_fprintf(int fd, const char *fmt, ...);
int io_vfprintf(int fd, const char *fmt, va_list ap);
bool io_writeall(int fd, const char *buf, size_t n);

#endif /* LIBSRSBSNS_IO_H */
