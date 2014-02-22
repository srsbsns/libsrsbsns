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


#include <libsrsbsns/addr.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <err.h>

int
addr_mksocket(const char *addr, unsigned short port, int socktype, int aflags, conbind_t func)
{
	struct addrinfo *ai_list = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = socktype;//SOCK_STREAM
	hints.ai_protocol = 0;
	hints.ai_flags = AI_NUMERICSERV | aflags;
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);

	int r = getaddrinfo(addr, portstr, &hints, &ai_list);

	if (r != 0) {
		warnx("%s", gai_strerror(r));
		return -1;
	}

	if (!ai_list) {
		warnx("result address list empty");
		return -1;
	}

	int sck = -1;

	for (struct addrinfo *ai = ai_list; ai; ai = ai->ai_next)
	{
		sck = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sck < 0) {
			warn("cannot create socket");
			continue;
		}

		if (func) {
			errno = 0;
			r = func(sck, ai->ai_addr, ai->ai_addrlen);

			if (r != 0)
			{
				warn("target function (connect/bind) failed");
				close(sck);
				sck = -1;
				continue;
			}
		}
		break;
	}

	freeaddrinfo(ai_list);

	return sck;
}

int
addr_connect_socket(const char *host, unsigned short port)
{
	return addr_mksocket(host, port, SOCK_STREAM, 0, connect);
}

int
addr_bind_socket(const char *localif, unsigned short port)
{
	return addr_mksocket(localif, port, SOCK_STREAM, AI_ADDRCONFIG | AI_PASSIVE, bind);
}

int
addr_connect_socket_dgram(const char *host, unsigned short port)
{
	return addr_mksocket(host, port, SOCK_DGRAM, 0, connect);
}

int
addr_bind_socket_dgram(const char *localif, unsigned short port, bool bc, bool ra)
{
	int s = addr_mksocket(localif, port, SOCK_DGRAM, AI_ADDRCONFIG | AI_PASSIVE, bind);
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

void addr_parse_hostspec(char *hoststr, size_t hoststr_sz,
		unsigned short *port, const char *hostspec)
{
	strncpy(hoststr, hostspec, hoststr_sz);
	char *ptr = strchr(hoststr, ']');
	if (!ptr)
		ptr = hoststr;
	ptr = strchr(ptr, ':');
	if (ptr) {
		*port = (unsigned short)strtol(ptr+1, NULL, 10);
		*ptr = '\0';
	} else
		*port = 0;
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
addr_make_sockaddr(const char *ip, struct sockaddr *dst)
{
	char host[256];
	unsigned short port;
	addr_parse_hostspec(host, sizeof host, &port, ip);
	
	int r;

	if (host[0] == '[') {
		host[strlen(host)-1] = '\0';
		r = addr_make_sockaddr_in6(host+1, port,
				(struct sockaddr_in6*)dst);
	} else
		r = addr_make_sockaddr_in4(host, port,
				(struct sockaddr_in*)dst);

	return r;
}
