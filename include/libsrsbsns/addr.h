/* addr.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#ifndef LIBSRSBSNS_ADDR_H
#define LIBSRSBSNS_ADDR_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>

typedef int (*conbind_t)(int, const struct sockaddr*, socklen_t);

/* backend */
int addr_mksocket(const char *host, const char *service,
    int socktype, int aflags, conbind_t func,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

/* split a hostspec ("host[:port]") where host may be FQDN, IPv4 addr or [IPv6] addr
 * store results in ,,hoststr'' and ,,port'' */
void addr_parse_hostspec(char *hoststr, size_t hoststr_sz, char *service, size_t service_sz, const char *hostspec);
void addr_parse_hostspec_p(char *hoststr, size_t hoststr_sz, unsigned short *port, const char *hostspec);
int addr_make_sockaddr(const char *ip, struct sockaddr *dst, size_t *sasz);

int addr_connect_socket_p(const char *host, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_bind_socket_p(const char *localif, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_connect_socket_dgram_p(const char *host, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_bind_socket_dgram_p(const char *localif, unsigned short port, bool bc, bool ra,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_connect_socket(const char *host, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_bind_socket(const char *localif, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_connect_socket_dgram(const char *host, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

int addr_bind_socket_dgram(const char *localif, const char *service, bool bc, bool ra,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us);

#endif /* LIBSRSBSNS_ADDR_H */
