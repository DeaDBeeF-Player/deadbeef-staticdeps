#!/bin/bash
./configure --disable-static --enable-shared $CONFIG_OPTS || exit 1
make
test -f src/.libs/libdispatch.so || exit 1