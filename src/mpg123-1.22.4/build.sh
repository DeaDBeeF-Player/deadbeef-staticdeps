autoreconf -i
./configure --disable-network --disable-gapless --disable-shared --enable-static --disable-messages --enable-new-huffman --enable-int-quality $CONFIG_OPTS CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" || exit 1
make clean
make -j8 CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" || exit 1
