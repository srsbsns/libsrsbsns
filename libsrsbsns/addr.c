/* addr.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

/*
struct addrinfo
memset
AF_UNSPEC
SOCK_STREAM
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

#include <libsrsbsns/log.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <netdb.h>


int
mksocket(const char *addr, unsigned short port, int aflags, conbind_t func)
{
	struct addrinfo *ai_list = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_NUMERICSERV | aflags;
	char portstr[6];
	snprintf(portstr, sizeof portstr, "%hu", port);

	int r = getaddrinfo(addr, portstr, &hints, &ai_list);

	if (r != 0) {
		W("%s", gai_strerror(r));
		return -1;
	}

	if (!ai_list) {
		W("result address list empty");
		return -1;
	}

	int sck = -1;

	for (struct addrinfo *ai = ai_list; ai; ai = ai->ai_next)
	{
		sck = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sck < 0) {
			WE("cannot create socket");
			continue;
		}

		if (func) {
			errno = 0;
			r = func(sck, ai->ai_addr, ai->ai_addrlen);

			if (r != 0)
			{
				WE("target function (connect/bind) failed");
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
connect_socket(const char *host, unsigned short port)
{
	return mksocket(host, port, 0, connect);
}

int
bind_socket(const char *localif, unsigned short port)
{
	return mksocket(localif, port, AI_ADDRCONFIG | AI_PASSIVE, bind);
}
