autoreconf -f -i
CFLAGS="-std=c99" ./configure --disable-test-coverage --enable-static --disable-sqlite --disable-alsa --disable-external-libs $CONFIG_OPTS
make clean
make

