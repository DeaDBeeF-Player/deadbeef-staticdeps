#!/bin/sh

./configure --without-http --without-regexps --without-modules --without-threads --enable-shared=no --enable-static $CONFIG_OPTS CC="$CC" CFLAGS="$CFLAGS -std=c89" LDFLAGS="$LDFLAGS"
make clean
make -j8 CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

