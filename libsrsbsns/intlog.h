/* log.h - (C) 2014, Timo Buhrmester
* libsrsbsns - A srs lib
* See README for contact-, COPYING for license information. */

#ifndef INTLOG_H
#define INTLOG_H 1

#include <stdbool.h>
#include <errno.h>

#include <syslog.h>

//[DINWE](): log with Debug, Info, Notice, Warn, Error severity.
//[DINWE]E(): similar, but also append ``errno'' message
 
// ----- logging interface -----

#define D(F,A...)                                               \
    bsnslog_log(LOG_DEBUG,-1,__FILE__,__LINE__,__func__,F,##A)

#define DE(F,A...)                                              \
    bsnslog_log(LOG_DEBUG,errno,__FILE__,__LINE__,__func__,F,##A)

#define I(F,A...)                                               \
    bsnslog_log(LOG_INFO,-1,__FILE__,__LINE__,__func__,F,##A)

#define IE(F,A...)                                              \
    bsnslog_log(LOG_INFO,errno,__FILE__,__LINE__,__func__,F,##A)

#define N(F,A...)                                               \
    bsnslog_log(LOG_NOTICE,-1,__FILE__,__LINE__,__func__,F,##A)

#define NE(F,A...)                                              \
    bsnslog_log(LOG_NOTICE,errno,__FILE__,__LINE__,__func__,F,##A)

#define W(F,A...)                                               \
    bsnslog_log(LOG_WARNING,-1,__FILE__,__LINE__,__func__,F,##A)

#define WE(F,A...)                                              \
    bsnslog_log(LOG_WARNING,errno,__FILE__,__LINE__,__func__,F,##A)

#define E(F,A...)                                               \
    bsnslog_log(LOG_ERR,-1,__FILE__,__LINE__,__func__,F,##A)

#define EE(F,A...)                                              \
    bsnslog_log(LOG_ERR,errno,__FILE__,__LINE__,__func__,F,##A)

// ----- logger control interface -----

void bsnslog_stderr(void);
void bsnslog_syslog(const char *ident, int facility);

void bsnslog_setlvl(int lvl);
int bsnslog_getlvl(void);

void bsnslog_setfancy(bool fancy);
bool bsnslog_getfancy(void);


// ----- backend -----
void bsnslog_log(int lvl, int errn, const char *file, int line,
    const char *func, const char *fmt, ...);

#endif /* INTLOG_H */
