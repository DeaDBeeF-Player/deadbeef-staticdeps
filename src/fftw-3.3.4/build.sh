./configure --enable-static --disable-shared --enable-sse2 --enable-avx $CONFIG_OPTS
make clean
make -j8
