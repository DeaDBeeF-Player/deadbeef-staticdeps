autoreconf -f -i
./configure --enable-static --disable-sndfile $CONFIG_OPTS
make clean
make
