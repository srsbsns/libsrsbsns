#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libsrsbsns], [0.0.1], [van.fisted@googlemail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/srstest.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
LT_PREREQ([2.4])
AM_PROG_AR
LT_INIT

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h stddef.h stdint.h stdlib.h string.h sys/socket.h sys/time.h syslog.h unistd.h])

AC_CHECK_LIB([m], [log2], [], [AC_MSG_ERROR([libm missing?])])
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT64_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T


# Checks for library functions.
AC_FUNC_STRERROR_R
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset select socket strchr strtol strtoul])

# Checks for header files (1).
AC_CONFIG_FILES([Makefile
		 include/Makefile
		 include/libsrsbsns/Makefile
		 libsrsbsns/Makefile
		 unittests/Makefile
                 src/Makefile])
AC_OUTPUT
echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
 Libraries: '${LIBS}'

--------------------------------------------------"
scripts/gentests.sh
