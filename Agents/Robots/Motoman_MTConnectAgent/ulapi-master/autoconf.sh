#!/bin/sh -x

aclocal && autoheader && automake --add-missing --copy && autoconf && ./configure CFLAGS="-g -Wall" && exit 0

exit 1


