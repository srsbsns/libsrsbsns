/* log.c - (C) 2014, Timo Buhrmester
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <libsrsbsns/misc.h>
#include <libsrsbsns/strmap.h>

#include "intlog.h"

#define COL_REDINV "\033[07;31;01m"
#define COL_RED "\033[31;01m"
#define COL_YELLOW "\033[33;01m"
#define COL_GREEN "\033[32;01m"
#define COL_WHITE "\033[37;01m"
#define COL_WHITEINV "\033[07;37;01m"
#define COL_GRAY "\033[30;01m"
#define COL_RST "\033[0m"


static bool s_open;
static bool s_stderr = true;
static int s_lvl = -1;
static smap_t s_lvlmap;
static bool s_fancy;

static const char* lvlnam(int lvl);
static const char* lvlcol(int lvl);


// ----- public interface implementation -----


void
bsnslog_syslog(const char *ident, int facility)
{
	if (s_open)
		closelog();
	openlog(ident, LOG_PID, facility);
	s_open = true;
	s_fancy = false;
	s_stderr = false;
}


void
bsnslog_stderr(void)
{
	if (s_open)
		closelog();
	
	s_stderr = true;
}


void
bsnslog_setfancy(bool fancy)
{
	if (!s_stderr)
		return; //don't send color sequences to syslog
	s_fancy = fancy;
}


bool
bsnslog_getfancy(void)
{
	return s_stderr && s_fancy;
}


void
bsnslog_setlvl(int lvl)
{
	s_lvl = lvl;
}


int
bsnslog_getlvl(void)
{
	return s_lvl;
}

static void
bsnslog_init(void)
{
	s_lvl = LOG_ERR;
	if (!(s_lvlmap = smap_init(16)))
		return;

	char tok[64];
	const char *v = getenv("LIBSRSBSNS_DEBUG");
	if (v) {
		const char *sp = v;
		do {
			while (isspace(*sp))
				sp++;

			if (!*sp)
				break;

			const char *end = strchr(sp, ' ');
			if (!end)
				end = sp + strlen(sp);

			size_t len = (size_t)(end - sp) + 1;
			if (len > sizeof tok)
				len = sizeof tok;

			strNcpy(tok, sp, len);
			sp = end;

			char *eq = strchr(tok, '=');

			if (!eq) {
				if (isdigitstr(tok)) {
					/* special case: a stray number
					 * means set default loglevel */
					s_lvl = (int)strtol(tok, NULL, 10);
				}
				continue;
			}

			char nam[64], val[64];
			size_t nlen = eq - tok + 1;
			if (nlen > sizeof nam)
				nlen = sizeof nam;

			size_t vlen = strlen(eq); // neither +1 nor -1
			if (vlen > sizeof val)
				vlen = sizeof val;

			strNcpy(nam, tok, nlen);
			strNcpy(val, eq + 1, vlen);

			if (!isdigitstr(val))
				continue;

			//fprintf(stderr, "tok '%s', nam: '%s', val: '%s'",
			//    tok, nam, val);
			int *i = malloc(sizeof i);
			if (!i)
				continue;

			*i = (int)strtol(val, NULL, 10);
			smap_put(s_lvlmap, nam, i);
		} while (*sp);
	}

	v = getenv("LIBSRSBSNS_DEBUG_TARGET");
	if (v && strcmp(v, "syslog") == 0)
		bsnslog_syslog("libsrsbsns", LOG_USER);
	else
		bsnslog_stderr();

	v = getenv("LIBSRSBSNS_DEBUG_FANCY");
	if (v && v[0] != '0')
		bsnslog_setfancy(true);
	else
		bsnslog_setfancy(false);

}

void
bsnslog_log(int lvl, int errn, const char *file, int line, const char *func,
    const char *fmt, ...)
{
	if (s_lvl == -1)
		bsnslog_init();

	int thr = s_lvl;
	int *l;
	if (s_lvlmap && (l = smap_get(s_lvlmap, file)))
		thr = *l;

	if (lvl > thr)
		return;

	char buf[4096];

	va_list vl;
	va_start(vl, fmt);

	char errmsg[256];
	errmsg[0] = '\0';
	if (errn >= 0) {
		errmsg[0] = ':';
		errmsg[1] = ' ';
		strerror_r(errn, errmsg + 2, sizeof errmsg - 2);
	}

	if (s_stderr) {
		char timebuf[27];
		time_t t = time(NULL);
		if (!ctime_r(&t, timebuf))
			strcpy(timebuf, "(ctime_r() failed)");
		char *ptr = strchr(timebuf, '\n');
		if (ptr)
			*ptr = '\0';

		snprintf(buf, sizeof buf, "%s%s: libsrsbsns: %s: %s:%d:%s(): %s%s%s\n",
		    s_fancy ? lvlcol(lvl) : "", timebuf, lvlnam(lvl), file, line,
		    func, fmt, errmsg, s_fancy ? COL_RST : "");
		vfprintf(stderr, buf, vl);

	} else {
		snprintf(buf, sizeof buf, "%s:%d:%s(): %s%s",
		    file, line, func, fmt, errmsg);
		vsyslog(lvl, buf, vl);
	}

	va_end(vl);
}


// ---- local helpers ---- 


static const char*
lvlnam(int lvl)
{
	return lvl == LOG_DEBUG ? "DBG" :
	       lvl == LOG_INFO ? "INF" :
	       lvl == LOG_NOTICE ? "NOT" :
	       lvl == LOG_WARNING ? "WRN" :
	       lvl == LOG_ERR ? "ERR" : "???";
}

static const char*
lvlcol(int lvl)
{
	return lvl == LOG_DEBUG ? COL_GRAY :
	       lvl == LOG_INFO ? COL_WHITE :
	       lvl == LOG_NOTICE ? COL_GREEN :
	       lvl == LOG_WARNING ? COL_YELLOW :
	       lvl == LOG_ERR ? COL_RED : COL_WHITEINV;
}
