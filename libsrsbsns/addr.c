/* addr.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
 * See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

/*
struct addrinfo
memset
AF_UNSPEC
AI_NUMERICSERV
snprintf
getaddrinfo
gai_strerror
socket
errno
close
freeaddrinfo
AI_ADDRCONFIG
AI_PASSIVE
bind
connect
*/


#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <unistd.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <err.h>

#include <libsrsbsns/misc.h>
#include <libsrsbsns/io.h>

#include "intlog.h"

#include <libsrsbsns/addr.h>


static int
setblocking(int fd, bool blocking)
{
	int opts = fcntl(fd, F_GETFL);
	if (blocking)
		opts &= ~O_NONBLOCK;
	else
		opts |= O_NONBLOCK;
	return fcntl(fd, F_SETFL, opts);
}

int addr_mksocket(const char *host, const char *service,
    int socktype, int aflags, conbind_t func,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	D("invoked: host='%s', serv='%s', scktype: %d, aflags: %d, func: %s, sto=%lu, hto=%lu", host, service, socktype, aflags, !func ? "(none)" : func == connect ? "connect" : func == bind ? "bind" : "(unkonwn)", softto_us, hardto_us);

	struct addrinfo *ai_list = NULL;
	struct addrinfo hints;
	int64_t hardtsend = hardto_us ? tstamp_us() + hardto_us : 0;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = socktype;
	hints.ai_protocol = 0;
	hints.ai_flags = aflags;
	if (isdigitstr(service))
		hints.ai_flags |= AI_NUMERICSERV;


	D("calling getaddrinfo on '%s:%s'", host, service);

	int r = getaddrinfo(host, service, &hints, &ai_list);

	if (r != 0) {
		W("getaddrinfo() failed: %s", gai_strerror(r));
		return -1;
	}

	if (!ai_list) {
		W("getaddrinfo() result address list empty");
		return -1;
	}

	bool success = false;
	int sck = -1;

	D("iterating over result list...");
	for (struct addrinfo *ai = ai_list; ai; ai = ai->ai_next) {
		sck = -1;
		if (hardtsend && hardtsend - tstamp_us() <= 0) {
			W("hard timeout");
			return 0;
		}

		D("next result, creating socket (fam=%d, styp=%d, prot=%d)",
		    ai->ai_family, ai->ai_socktype, ai->ai_protocol);

		sck = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sck < 0) {
			WE("cannot create socket");
			continue;
		}

		char peeraddr[64] = "(non-INET/INET6)";
		unsigned short peerport = 0;

		if (ai->ai_family == AF_INET) {
			struct sockaddr_in *sin =
			    (struct sockaddr_in*)ai->ai_addr;

			inet_ntop(AF_INET, &sin->sin_addr,
			    peeraddr, sizeof peeraddr);

			peerport = ntohs(sin->sin_port);
		} else if (ai->ai_family == AF_INET6) {
			struct sockaddr_in6 *sin =
			    (struct sockaddr_in6*)ai->ai_addr;

			inet_ntop(AF_INET6, &sin->sin6_addr,
			    peeraddr, sizeof peeraddr);

			peerport = ntohs(sin->sin6_port);
		}

		char portstr[7];
		snprintf(portstr, sizeof portstr, ":%hu", peerport);
		strNcat(peeraddr, portstr, sizeof peeraddr);

		int opt = 1;
		socklen_t optlen = sizeof opt;

		D("peer addr is '%s'", peeraddr);

		if (sockaddr)
			*sockaddr = *(ai->ai_addr);
		if (addrlen)
			*addrlen = ai->ai_addrlen;
		
		if (func) {
			D("going non-blocking");
			if (setblocking(sck, false) == -1) {
				WE("failed to enable nonblocking mode");
				close(sck);
				continue;
			}

			D("set to nonblocking mode, calling the backend function...");
			errno = 0;
			bool doselect = false;
			r = func(sck, ai->ai_addr, ai->ai_addrlen);

			if (r == -1 && (errno != EINPROGRESS)) {
				WE("backend failed");
				close(sck);
				continue;
			} else if (r == -1)
				doselect = true;

			int64_t trem = 0;
			D("backend returned with %d", r);
			if (doselect) {
				if (hardtsend) {
					trem = hardtsend - tstamp_us();
					if (trem <= 0) {
						D("hard timeout detected");
						close(sck);
						continue;
					}
				}

				int64_t softtsend = softto_us ? tstamp_us() + softto_us : 0;

				if (softtsend) {
					int64_t trem_tmp = softtsend - tstamp_us();

					if (trem_tmp <= 0) {
						W("soft timeout");
						close(sck);
						continue;
					}

					if (trem_tmp < trem)
						trem = trem_tmp;
				}

				D("calling io_select1w (timeout: %lld us)", trem);
				r = io_select1w(sck, trem, true);

				if (r < 0) {
					WE("select() failed");
					close(sck);
					continue;
				} else if (!r) {
					W("select() timeout");
					close(sck);
					continue;
				} else
					D("selected!");

				D("calling getsockopt to query error state");
				if (getsockopt(sck, SOL_SOCKET, SO_ERROR, &opt, &optlen) != 0) {
					W("getsockopt failed");
					close(sck);
					continue;
				}

				if (opt == 0) {
					I("socket in good shape! ('%s')", peeraddr);
					if (setblocking(sck, true) == -1) {
						WE("failed to disable nonblocking mode");
						close(sck);
						continue;
					}
					success = true;
					break;
				} else {
					char errstr[256];
					strerror_r(opt, errstr, sizeof errstr);
					W("backend function failed (%d: %s)", opt, errstr);
					close(sck);
					continue;
				}
			} else {
				I("There we go... ('%s')", peeraddr);
				if (setblocking(sck, true) == -1) {
					WE("failed to disable nonblocking mode");
					close(sck);
					continue;
				}
				success = true;
				break;
			}

		} else {
			success = true;
			break;
		}
	}
	
	D("after loop; alling freeaddrinfo then returning %d", sck);

	freeaddrinfo(ai_list);

	return success ? sck : -1;
}

int
addr_connect_socket_p(const char *host, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);
	return addr_connect_socket(host, portstr, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_bind_socket_p(const char *localif, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);
	return addr_bind_socket(localif, portstr, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_connect_socket_dgram_p(const char *host, unsigned short port,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);
	return addr_connect_socket_dgram(host, portstr, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_bind_socket_dgram_p(const char *localif, unsigned short port, bool bc, bool ra,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);
	return addr_bind_socket_dgram(localif, portstr, bc, ra, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_connect_socket(const char *host, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	return addr_mksocket(host, service, SOCK_STREAM, 0, connect, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_bind_socket(const char *localif, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	return addr_mksocket(localif, service, SOCK_STREAM, AI_ADDRCONFIG | AI_PASSIVE, bind, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_connect_socket_dgram(const char *host, const char *service,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	return addr_mksocket(host, service, SOCK_DGRAM, 0, connect, sockaddr, addrlen, softto_us, hardto_us);
}

int
addr_bind_socket_dgram(const char *localif, const char *service, bool bc, bool ra,
    struct sockaddr *sockaddr, size_t *addrlen,
    int64_t softto_us, int64_t hardto_us)
{
	int s = addr_mksocket(localif, service, SOCK_DGRAM, AI_ADDRCONFIG | AI_PASSIVE, bind, sockaddr, addrlen, softto_us, hardto_us);
	if (s >= 0 && bc) {
		int on = 1;
		errno = 0;
		if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != 0) {
			warn("setsockopt failed to enable broadcast");
			close(s);
			s = -1;
		}
	}
	if (s >= 0 && ra) {
		int on = 1;
		errno = 0;
		if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != 0) {
			warn("setsockopt failed to enable broadcast");
			close(s);
			s = -1;
		}

		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
			warn("setsockopt failed to enable REUSEADDR");
			/*not so critical, we can continue*/
		}
	}
	return s;
}

void addr_parse_hostspec_p(char *hoststr, size_t hoststr_sz,
		unsigned short *port, const char *hostspec)
{
	char portstr[6];
	addr_parse_hostspec(hoststr, hoststr_sz, portstr, sizeof portstr, hostspec);
	if (port)
		*port = (unsigned short)strtoul(portstr, NULL, 10);
}

void addr_parse_hostspec(char *hoststr, size_t hoststr_sz,
		char *service, size_t service_sz, const char *hostspec)
{
	strNcpy(hoststr, hostspec + (hostspec[0] == '['), hoststr_sz);
	char *ptr = strchr(hoststr, ']');
	if (!ptr)
		ptr = hoststr;
	else
		*ptr++ = '\0';

	ptr = strchr(ptr, ':');
	if (ptr) {
		if (service && service_sz)
			strNcpy(service, ptr+1, service_sz);
		*ptr = '\0';
	} else {
		if (service && service_sz)
			service[0] = '\0';
	}
}


static bool
addr_make_sockaddr_in6(const char *ip, unsigned short port,
		struct sockaddr_in6 *dst)
{
	errno = 0;
	int r = inet_pton(AF_INET6, ip, &dst->sin6_addr);
	if (r == 0) {
		warnx("illegal ipv6 string '%s'", ip);
		return false;
	}

	if (r < 0) {
		warn("inet_pton");
		return false;
	}

	dst->sin6_family = AF_INET6;
	dst->sin6_port = htons(port);
	return true;
}


static bool
addr_make_sockaddr_in4(const char *ip, unsigned short port,
		struct sockaddr_in *dst)
{
	errno = 0;
	int r = inet_pton(AF_INET, ip, &dst->sin_addr);
	if (r == 0) {
		warnx("illegal ipv4 string '%s'", ip);
		return false;
	}

	if (r < 0) {
		warn("inet_pton");
		return false;
	}

	dst->sin_family = AF_INET;
	dst->sin_port = htons(port);
	return true;
}


/* ip format: "x.y.z.w:port" for ipv4, "[a:b:c::h]:port" for ipv6" */
int
addr_make_sockaddr(const char *ip, struct sockaddr *dst, size_t *sasz)
{
	char host[256];
	unsigned short port;
	addr_parse_hostspec_p(host, sizeof host, &port, ip);
	
	int r;

	if (strchr(host, ':')) {
		host[strlen(host)-1] = '\0';
		r = addr_make_sockaddr_in6(host+1, port,
				(struct sockaddr_in6*)dst);
		if (sasz)
			*sasz = sizeof (struct sockaddr_in6);
	} else {
		r = addr_make_sockaddr_in4(host, port,
				(struct sockaddr_in*)dst);
		if (sasz)
			*sasz = sizeof (struct sockaddr_in);
	}
	return r;
}
