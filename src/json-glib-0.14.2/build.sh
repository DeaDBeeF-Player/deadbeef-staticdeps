#!/bin/sh
export JSON_CFLAGS="${GLIB_CFLAGS}"
export JSON_LIBS="${GLIB_LIBS}"
echo JSON_CFLAGS: $JSON_CFLAGS
./configure --enable-static $CONFIG_OPTS
make clean
make
