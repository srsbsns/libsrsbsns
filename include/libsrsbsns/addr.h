/* addr.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef LIBSRSBSNS_ADDR_H
#define LIBSRSBSNS_ADDR_H

#include <stdbool.h>
#include <sys/socket.h>

typedef int (*conbind_t)(int, const struct sockaddr*, socklen_t);

int addr_connect_socket(const char *host, unsigned short port);
int addr_bind_socket(const char *localif, unsigned short port);

int addr_connect_socket_dgram(const char *host, unsigned short port);
int addr_bind_socket_dgram(const char *localif, unsigned short port, bool bc, bool ra);

/* backend */
int addr_mksocket(const char *addr, unsigned short port, int socktype, int aflags, conbind_t func);

/* split a hostspec ("host[:port]") where host may be FQDN, IPv4 addr or [IPv6] addr
 * store results in ,,hoststr'' and ,,port'' */
void addr_parse_hostspec(char *hoststr, size_t hoststr_sz, unsigned short *port, const char *hostspec);
int addr_make_sockaddr(const char *ip, struct sockaddr *dst);
#endif /* LIBSRSBSNS_ADDR_H */
