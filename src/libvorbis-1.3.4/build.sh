CFLAGS="$CFLAGS -fno-finite-math-only"
./configure --enable-static --disable-shared $CONFIG_OPTS
make clean
make -j8
