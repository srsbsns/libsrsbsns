/* log.h - (C) 2012, Timo Buhrmester
 * libsrsbsns - A srs lib
  * See README for contact-, COPYING for license information.  */

#ifndef FSTD_LOG_H
#define FSTD_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#define LOGLVL_ERR 0
#define LOGLVL_WARN 1
#define LOGLVL_NOTE 2
#define LOGLVL_DBG 3

#define LOG_LEVEL(LVL) do{log_set_level(__FILE__, (LVL));}while(0)
#define LOG_FANCY(FCY) do{log_set_fancy(__FILE__, (FCY));}while(0)
#define LOG_THRNAME(NAME) do{log_set_thrname(pthread_self(), (NAME));}while(0)

#define LOG_GET_LEVEL log_get_level(__FILE__)
#define LOG_IS_FANCY log_is_fancy(__FILE__)
#define LOG_GET_THRNAME log_get_thrname(pthread_self())

/*explanation:                                                            
 * macro naming convention is V?[EWND][CE]? (regex)                          
 * the V* class of macros is meant to be called with a va_list argument.     
 * E(rror), W(arning), N(otice) and D(ebug) denote the log level             
 * the *C-flavour of these macros is supplied with an integer representing an errno-value whose string rep. is printed out
 * the *E-flavour will instead use the errno-value of the actual errno       
 * the plain group with neither E nor C disregards errno                     
 * the E*-functions are special in that they eventually call exit()          
 */                                       

#define E(FMT,ARGS...)     xerrx (__FILE__, __LINE__, __func__, EXIT_FAILURE,       (FMT), ##ARGS)
#define EC(EC,FMT,ARGS...) xerrc (__FILE__, __LINE__, __func__, EXIT_FAILURE, (EC), (FMT), ##ARGS)
#define EE(FMT,ARGS...)    xerr  (__FILE__, __LINE__, __func__, EXIT_FAILURE,       (FMT), ##ARGS)
#define W(FMT,ARGS...)     xwarnx(__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define WC(EC,FMT,ARGS...) xwarnc(__FILE__, __LINE__, __func__,               (EC), (FMT), ##ARGS)
#define WE(FMT,ARGS...)    xwarn (__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define N(FMT,ARGS...)     xnotex(__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define NC(EC,FMT,ARGS...) xnotec(__FILE__, __LINE__, __func__,               (EC), (FMT), ##ARGS)
#define NE(FMT,ARGS...)    xnote (__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define D(FMT,ARGS...)     xdbgx (__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define DC(EC,FMT,ARGS...) xdbgc (__FILE__, __LINE__, __func__,               (EC), (FMT), ##ARGS)
#define DE(FMT,ARGS...)    xdbg  (__FILE__, __LINE__, __func__,                     (FMT), ##ARGS)
#define VE(FMT,ARGS)      xverrx (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VEC(EC,FMT,ARGS)  xverrc (__FILE__, __LINE__, __func__,               (EC), (FMT), (ARGS))
#define VEE(FMT,ARGS)     xverr  (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VW(FMT,ARGS)      xvwarnx(__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VWC(EC,FMT,ARGS)  xvwarnc(__FILE__, __LINE__, __func__,               (EC), (FMT), (ARGS))
#define VWE(FMT,ARGS)     xvwarn (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VN(FMT,ARGS)      xvnotex(__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VNC(EC,FMT,ARGS)  xvnotec(__FILE__, __LINE__, __func__,               (EC), (FMT), (ARGS))
#define VNE(FMT,ARGS)     xvnote (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VD(FMT,ARGS)      xvdbgx (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))
#define VDC(EC,FMT,ARGS)  xvdbgc (__FILE__, __LINE__, __func__,               (EC), (FMT), (ARGS))
#define VDE(FMT,ARGS)     xvdbg  (__FILE__, __LINE__, __func__,                     (FMT), (ARGS))

void log_set_level(const char *file, int lvl);
int log_get_level(const char *file);

void log_set_fancy(const char *file, bool fancy);
bool log_is_fancy(const char *file);

void log_set_str(FILE *str);
FILE* log_get_str(void);

void log_set_timeoff(time_t timeoff);
time_t log_get_timeoff(void);

int log_count_mods(void);
const char* log_get_mod(int index);

void log_set_thrname(pthread_t thr, const char *name);
const char* log_get_thrname(pthread_t thr);

/* ------ end of interface ------ */

void xerr(const char *file, int line, const char *func, int eval, const char *fmt, ...);
void xerrc(const char *file, int line, const char *func, int eval, int code, const char *fmt, ...);
void xerrx(const char *file, int line, const char *func, int eval, const char *fmt, ...);
void xverr (const char *file, int line, const char *func, int eval, const char *fmt, va_list args);
void xverrc(const char *file, int line, const char *func, int eval, int code, const char *fmt, va_list args);
void xverrx(const char *file, int line, const char *func, int eval, const char *fmt, va_list args);

void xwarn(const char *file, int line, const char *func, const char *fmt, ...);
void xwarnc(const char *file, int line, const char *func, int code, const char *fmt, ...);
void xwarnx(const char *file, int line, const char *func, const char *fmt, ...);
void xvwarn (const char *file, int line, const char *func, const char *fmt, va_list args);
void xvwarnc(const char *file, int line, const char *func, int code, const char *fmt, va_list args);
void xvwarnx(const char *file, int line, const char *func, const char *fmt, va_list args);

void xnote(const char *file, int line, const char *func, const char *fmt, ...);
void xnotec(const char *file, int line, const char *func, int code, const char *fmt, ...);
void xnotex(const char *file, int line, const char *func, const char *fmt, ...);
void xvnote (const char *file, int line, const char *func, const char *fmt, va_list args);
void xvnotec(const char *file, int line, const char *func, int code, const char *fmt, va_list args);
void xvnotex(const char *file, int line, const char *func, const char *fmt, va_list args);

void xdbg(const char *file, int line, const char *func, const char *fmt, ...);
void xdbgc(const char *file, int line, const char *func, int code, const char *fmt, ...);
void xdbgx(const char *file, int line, const char *func, const char *fmt, ...);
void xvdbg (const char *file, int line, const char *func, const char *fmt, va_list args);
void xvdbgc(const char *file, int line, const char *func, int code, const char *fmt, va_list args);
void xvdbgx(const char *file, int line, const char *func, const char *fmt, va_list args);

#endif /* FSTD_LOG_H */
