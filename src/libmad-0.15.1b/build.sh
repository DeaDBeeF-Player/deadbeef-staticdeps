make distclean
./configure --enable-static --disable-shared --enable-speed --enable-fpm=intel $CONFIG_OPTS
make clean
make CFLAGS="-O2 -fomit-frame-pointer -fPIC"
