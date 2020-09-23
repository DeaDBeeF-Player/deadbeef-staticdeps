#!/bin/bash
./configure --disable-static --enable-shared $CONFIG_OPTS
make
test -f src/.libs/libdispatch.so || exit 1
