#!/bin/sh
make clean
make distclean
rm -f Makefile.in
rm -f aclocal.m4
rm -rf autom4te.cache/
rm -rf build-aux
rm -rf m4
rm -f config.h.in
rm -f 'config.h.in~'
rm -f configure
rm -f include/Makefile.in
rm -f include/libsrsbsns/Makefile.in
rm -f libsrsbsns/Makefile.in
rm -f src/Makefile.in
rm -f unittests/Makefile.in
rm -f unittests/run_*.c
