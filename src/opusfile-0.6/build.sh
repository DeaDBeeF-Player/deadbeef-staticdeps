autoreconf
./configure --enable-static --disable-shared --disable-http --disable-doc $CONFIG_OPTS
make clean
make -j8
