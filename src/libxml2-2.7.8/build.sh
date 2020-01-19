#!/bin/sh

./configure --without-http --without-regexps --without-modules --enable-shared=no --enable-static $CONFIG_OPTS CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"
make clean
make -j8 CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

