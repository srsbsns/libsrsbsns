/* log.h - (C) 2014, Timo Buhrmester
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#ifndef LOG_H
#define LOG_H 1

#include <stdbool.h>
#include <errno.h>

#include <syslog.h>

//[DINWEC](): log with Debug, Info, Notice, Warn, Error, Critical severity.
// Critical will also call exit(EXIT_FAILURE)
//[DINWEC]E(): similar, but also append ``errno'' message
 
// ----- logging interface -----

#define D(F,A...)                                               \
    log_log(LOG_DEBUG,-1,__FILE__,__LINE__,__func__,F,##A)

#define DE(F,A...)                                              \
    log_log(LOG_DEBUG,errno,__FILE__,__LINE__,__func__,F,##A)

#define I(F,A...)                                               \
    log_log(LOG_INFO,-1,__FILE__,__LINE__,__func__,F,##A)

#define IE(F,A...)                                              \
    log_log(LOG_INFO,errno,__FILE__,__LINE__,__func__,F,##A)

#define N(F,A...)                                               \
    log_log(LOG_NOTICE,-1,__FILE__,__LINE__,__func__,F,##A)

#define NE(F,A...)                                              \
    log_log(LOG_NOTICE,errno,__FILE__,__LINE__,__func__,F,##A)

#define W(F,A...)                                               \
    log_log(LOG_WARNING,-1,__FILE__,__LINE__,__func__,F,##A)

#define WE(F,A...)                                              \
    log_log(LOG_WARNING,errno,__FILE__,__LINE__,__func__,F,##A)

#define E(F,A...)                                               \
    log_log(LOG_ERR,-1,__FILE__,__LINE__,__func__,F,##A)

#define EE(F,A...)                                              \
    log_log(LOG_ERR,errno,__FILE__,__LINE__,__func__,F,##A)

#define C(F,A...)                                              \
    do{log_log(LOG_CRIT,-1,__FILE__,__LINE__,__func__,F,##A);    \
                      exit(EXIT_FAILURE); }while(0)
#define CE(F,A...)                                             \
    do{log_log(LOG_CRIT,errno,__FILE__,__LINE__,__func__,F,##A); \
                       exit(EXIT_FAILURE); }while(0)

// ----- logger control interface -----

void log_stderr(void);
void log_syslog(const char *ident, int facility);

void log_setlvl(int lvl);
int log_getlvl(void);

void log_setfancy(bool fancy);
bool log_getfancy(void);


// ----- backend -----
void log_log(int lvl, int errn, const char *file, int line,
    const char *func, const char *fmt, ...);

#endif /* LOG_H */
