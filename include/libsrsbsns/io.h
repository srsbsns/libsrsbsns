/* io.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_IO_H
#define LIBSRSBSNS_IO_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

int io_read_line(int fd, char *dest, size_t dest_sz);
int io_fprintf(int fd, const char *fmt, ...);
int io_vfprintf(int fd, const char *fmt, va_list ap);
bool io_writeall(int fd, const char *buf, size_t n);

int io_select1w(int fd, int64_t to_us, bool ignoreintr);
int io_select1r(int fd, int64_t to_us, bool ignoreintr);
int io_select2r(bool *rdbl1, bool *rdbl2, int fd1, int fd2,
    uint64_t to_us, bool ignoreintr);
int io_select(int *rfd, size_t num_rfd, int *wfd, size_t num_wfd,
           int *efd, size_t num_efd, int64_t to_us, bool ignoreintr);

#endif /* LIBSRSBSNS_IO_H */
