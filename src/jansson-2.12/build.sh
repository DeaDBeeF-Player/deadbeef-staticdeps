autoreconf -f -i
./configure --disable-shared --enable-static $CONFIG_OPTS
make clean
make

