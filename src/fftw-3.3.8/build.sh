#!/bin/bash

if [[ "$ARCH" == "i686" ]]; then
    OPT="--enable-sse2 --enable-avx"
elif [[ "$ARCH" == "x86_64" ]]; then
    OPT="--enable-sse2 --enable-avx"
elif [[ "$ARCH" == "aarch64" ]]; then
    OPT="--enable-neon"
else
    echo "Invalid arch: $ARCH"
    OPT=""
fi

./configure --enable-static --disable-shared $OPT $CONFIG_OPTS
make clean
make -j8
