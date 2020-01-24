#!/bin/sh

autoreconf
automake --add-missing
./configure --disable-alisp --enable-shared --disable-static $CONFIG_OPTS CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"
make clean
make -j8 CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

