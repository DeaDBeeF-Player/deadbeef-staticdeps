#!/bin/sh
export JSON_CFLAGS="${GLIB_CFLAGS}"
export JSON_LIBS="${GLIB_LIBS}"
./configure --enable-static $CONFIG_OPTS
make clean
make
