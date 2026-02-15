./configure --enable-static --with-zlib-prefix=../../deadbeef/lib-x86-32 $CONFIG_OPTS
make clean
make CFLAGS=-fPIC -j8

