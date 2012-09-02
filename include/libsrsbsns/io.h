/* io.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_IO_H
#define LIBSRSBSNS_IO_H

#include <stdbool.h>
#include <stddef.h>

int read_line(int fd, char *dest, size_t dest_sz);

void io_log_init(void);
void io_log_level(int loglvl);
void io_log_fancy(bool colors);

#endif /* LIBSRSBSNS_IO_H */
