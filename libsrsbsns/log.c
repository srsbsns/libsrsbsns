/* log.c - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libsrsbsns/log.h>

#include <common.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>
#include <time.h>


#define LOGBUFSZ 2048
#define DEF_LOGLVL LOGLVL_ERR
#define DEF_STR stderr
#define DEF_FANCY false

#define COLSTR_RED "\033[31;01m"
#define COLSTR_YELLOW "\033[33;01m"
#define COLSTR_GREEN "\033[32;01m"
#define COLSTR_GRAY "\033[30;01m"


struct thrlist_s {
	pthread_t thr;
	char *name;
	struct thrlist_s *next;
};

struct ctxlist_s {
	struct logctx_s *ctx;
	struct ctxlist_s *next;
};

struct logctx_s {
	int loglevel;
	char *mod;
	bool fancy;
};


static struct thrlist_s *s_thrlist;
static struct ctxlist_s *s_ctxlist;
static FILE *s_outstr;
static time_t s_timeoff;
static pthread_mutex_t s_mtx;
static bool s_mtxinit;


static void vlogf(const char *file, int line, const char *func, int lvl,
                                               const char *fmt, va_list l);
static const char* levtag(int lvl);
static const char* colstr(int lvl);
static struct logctx_s* findctx(const char *file);
static void addctx(struct logctx_s *ctx);
static struct logctx_s* getctx(const char *file);
static struct logctx_s* log_register(const char *mod, int lvl, bool fancy);
static pthread_mutex_t* mtx(void);


void
log_set_level(const char *file, int lvl)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	getctx(file)->loglevel = lvl;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
}


int
log_get_level(const char *file)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	int i = getctx(file)->loglevel;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return i;
}


void
log_set_fancy(const char *file, bool fancy)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	getctx(file)->fancy = fancy;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
}


bool
log_is_fancy(const char *file)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	bool b = getctx(file)->fancy;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return b;
}


void
log_set_str(FILE *str)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	s_outstr = str;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
}


FILE*
log_get_str(void)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	FILE *str = s_outstr;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return str;
}


void
log_set_timeoff(time_t timeoff)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	s_timeoff = timeoff;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
}


time_t
log_get_timeoff(void)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	time_t i = s_timeoff;
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return i;
}


int
log_count_mods(void)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	struct ctxlist_s *node = s_ctxlist;
	int i = 0;
	while(node) {
		i++;
		node = node->next;
	}
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return i;
}


const char*
log_get_mod(int index)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	struct ctxlist_s *node = s_ctxlist;
	while(node && index--)
		node = node->next;

	if (!node) {
		ENSURE(pthread_mutex_unlock(mtx()), 0);
		return NULL;
	}
	
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return node->ctx->mod;
}


void
log_set_thrname(pthread_t thr, const char *name)
{
	struct thrlist_s *newnode = malloc(sizeof *newnode);
	newnode->name = strdup(name);
	newnode->thr = thr;
	newnode->next = NULL;

	ENSURE(pthread_mutex_lock(mtx()), 0);

	if (!s_thrlist)
		s_thrlist = newnode;
	else {
		struct thrlist_s *node = s_thrlist;

		while(node->next) {
			if (node->thr == thr) {
				free(node->name);
				node->name = strdup(name);
				free(newnode);
				ENSURE(pthread_mutex_unlock(mtx()), 0);
				return;
			}
			node = node->next;
		}
		
		node->next = newnode;
	}
	ENSURE(pthread_mutex_unlock(mtx()), 0);
}


const char*
log_get_thrname(pthread_t thr)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	struct thrlist_s *node = s_thrlist;
	while(node) {
		if (node->thr == thr) {
			ENSURE(pthread_mutex_unlock(mtx()), 0);
			return node->name;
		}
		node = node->next;
	}
	ENSURE(pthread_mutex_unlock(mtx()), 0);
	return NULL;
}


static void
vlogf(const char *file, int line, const char *func, int lvl,
                                                const char *fmt, va_list l)
{
	ENSURE(pthread_mutex_lock(mtx()), 0);
	if (!s_outstr)
		s_outstr = DEF_STR;
	struct logctx_s *ctx = getctx(file);
	if (lvl > ctx->loglevel) { //should maybe ditch it earlier
		ENSURE(pthread_mutex_unlock(mtx()), 0);
		return;
	}
	char b[LOGBUFSZ];
	const char *tname = log_get_thrname(pthread_self());
	if (!tname) {
		static int tid = 0;
		snprintf(b, sizeof b, "t-%d", tid++);
		log_set_thrname(pthread_self(), b);
		tname = log_get_thrname(pthread_self());
	}

	static int s_maxtw, s_maxmw, s_maxfw, s_maxlw, s_maxline;

	if (strlen(tname) > (size_t)s_maxtw)
		s_maxtw = strlen(tname);

	if (strlen(ctx->mod) > (size_t)s_maxmw)
		s_maxmw = strlen(ctx->mod);
	
	if (strlen(func) > (size_t)s_maxfw)
		s_maxfw = strlen(func);

	if (line > s_maxline) {
		s_maxline = line;
		char buf[32];
		snprintf(buf, sizeof buf, "%d", line);
		s_maxlw = (int)strlen(buf);
	}

	unsigned long t = (unsigned long)(time(NULL) - s_timeoff);
	snprintf(b, sizeof b, "%s(%lu) [%*.*s] %-*.*s: %*.*s():%-*.*d: ",
	               (ctx->fancy ? colstr(lvl):levtag(lvl)), t,
	               s_maxtw, s_maxtw, tname, s_maxmw, s_maxmw, ctx->mod,
	               s_maxfw, s_maxfw, func, s_maxlw, s_maxlw, line);

	size_t len = strlen(b);
	vsnprintf(b + len, sizeof b - len, fmt, l);
	if (ctx->fancy)
		strNcat(b, "\033[0m", sizeof b);
	strNcat(b, "\n", sizeof b);
	FILE *str = s_outstr;
	ENSURE(pthread_mutex_unlock(mtx()), 0);

	fputs(b, str);
}


void
xerr(const char *file, int line, const char *func, int eval,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xverr(file, line, func, eval, fmt, l);
	va_end(l);
}


void
xerrc(const char *file, int line, const char *func, int eval, int code,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xverrc(file, line, func, eval, code, fmt, l);
	va_end(l);
}


void
xerrx(const char *file, int line, const char *func, int eval,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xverrx(file, line, func, eval, fmt, l);
	va_end(l);
}


void
xverr (const char *file, int line, const char *func, int eval,
                                             const char *fmt, va_list args)
{
	xverrc(file, line, func, eval, errno, fmt, args);
}


void
xverrc(const char *file, int line, const char *func, int eval, int code,
                                             const char *fmt, va_list args)
{
	char buf[LOGBUFSZ];
	strncpy(buf, fmt, sizeof buf);
	strNcat(buf, ": ", sizeof buf);
	size_t len = strlen(buf);
	strerror_r(code, buf + len, sizeof buf - len);
	xverrx(file, line, func, eval, buf, args);
}


void
xverrx(const char *file, int line, const char *func, int eval,
                                             const char *fmt, va_list args)
{
	vlogf(file, line, func, LOGLVL_ERR, fmt, args);
	exit(eval);
}


void
xwarn(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvwarn(file, line, func, fmt, l);
	va_end(l);
}


void
xwarnc(const char *file, int line, const char *func, int code,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvwarnc(file, line, func, code, fmt, l);
	va_end(l);
}


void
xwarnx(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvwarnx(file, line, func, fmt, l);
	va_end(l);
}


void
xvwarn (const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	xvwarnc(file, line, func, errno, fmt, args);
}


void
xvwarnc(const char *file, int line, const char *func, int code,
                                             const char *fmt, va_list args)
{
	char buf[LOGBUFSZ];
	strncpy(buf, fmt, sizeof buf);
	strNcat(buf, ": ", sizeof buf);
	size_t len = strlen(buf);
	strerror_r(code, buf + len, sizeof buf - len);
	xvwarnx(file, line, func, buf, args);
}


void
xvwarnx(const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	vlogf(file, line, func, LOGLVL_WARN, fmt, args);
}


void
xnote(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvnote(file, line, func, fmt, l);
	va_end(l);
}


void
xnotec(const char *file, int line, const char *func, int code,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvnotec(file, line, func, code, fmt, l);
	va_end(l);
}


void
xnotex(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvnotex(file, line, func, fmt, l);
	va_end(l);
}


void
xvnote (const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	xvnotec(file, line, func, errno, fmt, args);
}


void
xvnotec(const char *file, int line, const char *func, int code,
                                             const char *fmt, va_list args)
{
	char buf[LOGBUFSZ];
	strncpy(buf, fmt, sizeof buf);
	strNcat(buf, ": ", sizeof buf);
	size_t len = strlen(buf);
	strerror_r(code, buf + len, sizeof buf - len);
	xvnotex(file, line, func, buf, args);
}


void
xvnotex(const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	vlogf(file, line, func, LOGLVL_NOTE, fmt, args);
}


void
xdbg(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvdbg(file, line, func, fmt, l);
	va_end(l);
}


void
xdbgc(const char *file, int line, const char *func, int code,
                                                      const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvdbgc(file, line, func, code, fmt, l);
	va_end(l);
}


void
xdbgx(const char *file, int line, const char *func, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
		xvdbgx(file, line, func, fmt, l);
	va_end(l);
}


void
xvdbg(const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	xvdbgc(file, line, func, errno, fmt, args);
}


void
xvdbgc(const char *file, int line, const char *func, int code,
                                             const char *fmt, va_list args)
{
	char buf[LOGBUFSZ];
	strncpy(buf, fmt, sizeof buf);
	strNcat(buf, ": ", sizeof buf);
	size_t len = strlen(buf);
	strerror_r(code, buf + len, sizeof buf - len);
	xvdbgx(file, line, func, buf, args);
}


void
xvdbgx(const char *file, int line, const char *func,
                                             const char *fmt, va_list args)
{
	vlogf(file, line, func, LOGLVL_DBG, fmt, args);
}


static const char*
levtag(int lvl)
{
	switch(lvl) {
		case LOGLVL_ERR: return "[ERR] ";
		case LOGLVL_WARN: return "[WRN] ";
		case LOGLVL_NOTE: return "[NOT] ";
		case LOGLVL_DBG: return "[DBG] ";
		default: return "[???]";
			
	}
}


static const char*
colstr(int lvl)
{
	switch(lvl) {
		case LOGLVL_ERR: return COLSTR_RED;
		case LOGLVL_WARN: return COLSTR_YELLOW;
		case LOGLVL_NOTE: return COLSTR_GREEN;
		case LOGLVL_DBG: return COLSTR_GRAY;
		default: return "";
	}
}


static struct logctx_s*
findctx(const char *file)
{
	struct ctxlist_s *node = s_ctxlist;
	while(node) {
		if (strcmp(node->ctx->mod, file) == 0)
			return node->ctx;
		node = node->next;
	}
	return NULL;
}


static void
addctx(struct logctx_s *ctx)
{
	struct ctxlist_s *node = malloc(sizeof *node);
	if (!node)
		return;
	
	node->next = s_ctxlist;
	node->ctx = ctx;
	s_ctxlist = node;
}


static struct logctx_s*
getctx(const char *file)
{
	struct logctx_s *ctx = findctx(file);
	if (!ctx) {
		ctx = log_register(file, DEF_LOGLVL, DEF_FANCY);
		addctx(ctx);
	}
	return ctx;
}


static struct logctx_s*
log_register(const char *mod, int lvl, bool fancy)
{
	struct logctx_s *ctx = malloc(sizeof *ctx);
	ctx->loglevel = lvl;
	ctx->mod = strdup(mod);
	ctx->fancy = fancy;
	return ctx;
}

static pthread_mutex_t*
mtx(void)
{
	if (!s_mtxinit) {
		ENSURE(pthread_mutex_init(&s_mtx, NULL), 0);
		s_mtxinit = true;
	}
	return &s_mtx;
}
