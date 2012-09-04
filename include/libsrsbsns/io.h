/* io.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_IO_H
#define LIBSRSBSNS_IO_H

#include <stdbool.h>
#include <stddef.h>

int io_read_line(int fd, char *dest, size_t dest_sz);

#endif /* LIBSRSBSNS_IO_H */
